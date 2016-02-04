/*
 * skv_store.cpp
 *
 */

//#ifdef SKV_STORE

#include "skv_store.h"

#include <iostream>
#include <sstream>
#include <cstring>

#include <stdio.h>
#include <stdlib.h>


namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

#include <string>



KeyValueSkv::KeyValueSkv(int mpiRank, bool threadSafe, std::string pdsName) : KeyValueIface(), _rank(mpiRank), _skvState(SKV_NONE)
{
	if (threadSafe) {
		_lock = new MyOMPLock();
	} else {
		_lock = new MyDummyLock();
	}

	// The first thing for every client to do is to initialize SKV

	skv_status_t status = SKV_SUCCESS;

	/*****************************************************************************
	 * Init the SKV Client
	 ****************************************************************************/

	std::cout << "[" << _rank << "] initializing skv ibm" << std::endl;

	//if (_rank == 0)
	status = _skvClient.Init(0, MPI_COMM_WORLD, _rank);

	std::cout << "[" << _rank << "] skv initialized" << std::endl;

	MPI_Barrier(MPI_COMM_WORLD);

	_skvState = SKV_INIT;

	if (status != SKV_SUCCESS) {

		_skvState = SKV_NONE;
		std::cout
		<< "[" << _rank << "] SKV Client Init FAILED "
		<< " status: " << skv_status_to_string( status )
		<< std::endl;
	}
	/****************************************************************************/

	MPI_Barrier(MPI_COMM_WORLD);

	/*****************************************************************************
	 * Connect to the SKV Server
	 ****************************************************************************/
	std::cout
	<< "[" << _rank << "] Init_skv():: About to connect "
	//<< " ConfigFile: " << configFile
	<< std::endl;

	status = _skvClient.Connect( NULL, 0 );

	if (status == SKV_SUCCESS) {
		_skvState = SKV_CONNECT;
		std::cout
		<< "[" << _rank << "] SKV Client connected"
		<< std::endl;
	} else {
		std::cout
		<< "[" << _rank << "] SKV Client Connect FAILED to connect"
		//<< " config file: " << configFile
		<< " status: " << skv_status_to_string( status )
		<< std::endl;
	}
	/****************************************************************************/

	MPI_Barrier(MPI_COMM_WORLD);

	/*****************************************************************************
	 * Open a test PDS
	 ****************************************************************************/
	if (pdsName == "") {
		pdsName = "SKV_PDS";
	}

	status = _skvClient.Open( (char*)pdsName.c_str(),
			(skv_pds_priv_t) (SKV_PDS_READ | SKV_PDS_WRITE),
			(skv_cmd_open_flags_t) SKV_COMMAND_OPEN_FLAGS_CREATE,
			& _pdsId );

	if (status == SKV_SUCCESS) {
		_skvState = SKV_PDS;
		std::cout
		<< "[" << _rank << "] SKV Client PDS successfully opened: "
		<< pdsName
		<< " MyPDSId: " << _pdsId
		<< std::endl;
	} else {
		std::cout
		<< "[" << _rank << "] SKV Client PDS FAILED to open: "
		<< pdsName
		<< " status: " << skv_status_to_string( status )
		<< std::endl;
	}
	/****************************************************************************/

	MPI_Barrier(MPI_COMM_WORLD);
}


KeyValueSkv::~KeyValueSkv()
{

	skv_status_t status = SKV_ERRNO_UNSPECIFIED_ERROR;

	if (_skvState > SKV_PDS) {
		status = _skvClient.Close( &_pdsId );

		std::cout
		<< "[" << _rank << "] PDS closed with status: " << status
		<< std::endl;
	}

	MPI_Barrier(MPI_COMM_WORLD);

	if (_skvState >= SKV_CONNECT) {
		status = _skvClient.Disconnect();

		std::cout
		<< "[" << _rank << "] Disconnected with status: " << status
		<< std::endl;
	}

	MPI_Barrier(MPI_COMM_WORLD);

	if (_skvState >= SKV_INIT) {
		status = _skvClient.Finalize();

		std::cout
		<< "[" << _rank << "] Finalized with status: " << status
		<< std::endl;
	}
}



void KeyValueSkv::insert(const int * key, unsigned int keySize, const double * value, unsigned int valueSize, void * handle, bool async)
{
	skv_status_t status = SKV_ERRNO_UNSPECIFIED_ERROR;

	if (async) {

		if (handle == NULL) {
			std::cout << "Error: async operations need a non-NULL handle" << std::endl;
		}

		//std::cout << "[" << _rank << "] async Insert():: < " << *key << ", " << *value << " >" << std::endl;

		char * k = new char[keySize];
		std::memcpy(k, key, keySize);

		char * v = new char[valueSize];
		std::memcpy(v, value, valueSize);

		_lock->lock();
		status = _skvClient.iInsert(&_pdsId, k, sizeof(k), v, sizeof(v), 0,
				SKV_COMMAND_RIU_APPEND, (skv_client_cmd_ext_hdl_t *) handle);
		_lock->unlock();

		if (status == SKV_SUCCESS) {
			//std::cout
			//<< "[" << _rank << "] Async key-value pair insertion successful!"
			//<< std::endl;
		} else {
			std::cout
			<< "[" << _rank << "] Async key-value insertion FAILED: "
			<< " status: " << skv_status_to_string( status )
			<< std::endl;
		}

	} else {

		//std::cout << "[" << _rank << "] Insert():: < " << *key << ", " << *value << " >" << std::endl;

		_lock->lock();
		status = _skvClient.Insert(&_pdsId, (char *) key, keySize, (char *) value, valueSize, 0, SKV_COMMAND_RIU_APPEND );
		_lock->unlock();

		if (status == SKV_SUCCESS) {
			//std::cout
			//<< "[" << _rank << "] Key-value pair successfully inserted!"
			//<< std::endl;
		} else {
			std::cout
			<< "[" << _rank << "] Key-value insertion FAILED: "
			<< " status: " << skv_status_to_string( status )
			<< std::endl;
		}
	}
}




int KeyValueSkv::retrieve(const int * key, unsigned int keySize, double * value, unsigned int valueSize, void * handle, bool async)
{
	int size = 0;
	skv_status_t status = SKV_ERRNO_UNSPECIFIED_ERROR;

	if (async) {

		if (handle == NULL) {
			std::cout << "Error: async operations need a non-NULL handle" << std::endl;
		}

		//std::cout << "[" << _rank << "] async Retrieve():: key: " << key << std::endl;

		char * k = new char[keySize];
		std::memcpy(k, key, keySize);

		char * v = new char[valueSize];
		std::memcpy(v, value, valueSize);

		_lock->lock();
		status = _skvClient.iRetrieve(&_pdsId, k, sizeof(k), v, sizeof(v), &size, 0,
				SKV_COMMAND_RIU_FLAGS_NONE, (skv_client_cmd_ext_hdl_t *) handle);
		_lock->unlock();

		if (status == SKV_SUCCESS) {
			//std::cout
			//<< "[" << _rank << "] Async key-value pair successfully retrieved!"
			//<< std::endl;
		} else {
			std::cout
			<< "[" << _rank << "] Async key-value retrieve FAILED: "
			<< " status: " << skv_status_to_string( status )
			<< std::endl;skv_client_cmd_ext_hdl_t
	} else {
		//std::cout << "[" << _rank << "] Retrieve():: key: " << key << std::endl;

		_lock->lock();
		status = _skvClient.Retrieve(&_pdsId, (char *) key, keySize, (char *) value, valueSize, &size, 0, SKV_COMMAND_RIU_FLAGS_NONE );
		_lock->unlock();

		if (status == SKV_SUCCESS) {
			//std::cout
			//<< "[" << _rank << "] Key-value pair successfully retrieved!"
			//<< std::endl;
		} else {
			std::cout
			<< "[" << _rank << "] Key-value retrieve FAILED: "
			<< " status: " << skv_status_to_string( status )
			<< std::endl;
		}
	}

	return size;
}



void KeyValueSkv::remove(const int * key, unsigned int keySize, void * handle, bool async)
{
	skv_status_t status = SKV_ERRNO_UNSPECIFIED_ERROR;

	if (async) {

		if (handle == NULL) {
			std::cout << "Error: async operations need a non-NULL handle" << std::endl;
		}

		//std::cout << "[" << _rank << "] Async Remove():: key: " << *key << std::endl;

		char * k = new char[keySize];
		std::memcpy(k, key, keySize);

		_lock->lock();
		status = _skvClient.iRemove(&_pdsId, k, sizeof(k), SKV_COMMAND_REMOVE_FLAGS_NONE, (skv_client_cmd_ext_hdl_t *) handle);
		_lock->unlock();

		if (status == SKV_SUCCESS) {
			//std::cout
			//<< "[" << _rank << "] Async key-value pair successfully removed!"
			//<< std::endl;
		} else {
			std::cout
			<< "[" << _rank << "] Async key-value removal FAILED: "
			<< " status: " << skv_status_to_string( status )
			<< std::endl;
		}

	} else {
		std::cout << "[" << _rank << "] Remove():: key: " << key << std::endl;

		_lock->lock();
		status = _skvClient.Remove(&_pdsId, (char *) key, keySize, SKV_COMMAND_REMOVE_FLAGS_NONE);
		_lock->unlock();

		if (status == SKV_SUCCESS) {
			std::cout
			<< "[" << _rank << "] Key-value pair successfully removed!"
			<< std::endl;
		} else {
			std::cout
			<< "[" << _rank << "] Key-value removal FAILED: "
			<< " status: " << skv_status_to_string( status )
			<< std::endl;
		}
	}
}



void KeyValueSkv::wait(void * handle)
{
	skv_status_t status = SKV_ERRNO_UNSPECIFIED_ERROR;

	_lock->lock();
	status = _skvClient.Wait( *((skv_client_cmd_ext_hdl_t *) handle));
	_lock->unlock();

	if (status == SKV_SUCCESS) {
		//std::cout
		//<< "[" << _rank << "] Wait successfully completed!"
		//<< std::endl;
	} else {
		std::cout
		<< "[" << _rank << "] Wait FAILED: "
		<< " status: " << skv_status_to_string( status )
		<< std::endl;
	}

}

//#endif /* SKV_STORE */