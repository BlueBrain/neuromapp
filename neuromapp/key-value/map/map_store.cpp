/*
 * map_store.cpp
 *
 */

#include "map_store.h"

#include <iostream>
#include <cstring>
#include <sstream>
#include <vector>

KeyValueMap::KeyValueMap(int mpiRank, bool threadSafe, std::string pdsName) : KeyValueIface(), _rank(mpiRank), _numReaders(0), _numWriters(0){
	if (threadSafe) {
//		_nRdLock = new MyOMPLock();
//		_nWtLock = new MyOMPLock();
//		_readersLock = new MyOMPLock();
//		_writersLock = new MyOMPLock();
	} else {
		_nRdLock = new MyDummyLock();
		_nWtLock = new MyDummyLock();
		_readersLock = new MyDummyLock();
		_writersLock = new MyDummyLock();
	}
	std::cout << "[" << _rank << "] initialized skv map successfully" << std::endl;
}


KeyValueMap::~KeyValueMap(){
	_map.clear();
	_valSizes.clear();
	std::cout << "[" << _rank << "] Finalized successfully" << std::endl;
}



void KeyValueMap::insert(const int * key, unsigned int keySize, const double * value, unsigned int valueSize, void * handle, bool async)
{
	/****************************************/
	_readersLock->lock();
	while (_numReaders > 0) {}
	_writersLock->lock();
	while (_numReaders > 0) {}
	_nWtLock->lock();
	_numWriters++;
	_nWtLock->unlock();
	while (_numReaders > 0) {}
	/****************************************/
		int elems = valueSize/sizeof(double);
		std::vector<double> * v = new std::vector<double>(elems);
    
		for (int i = 0; i < elems; i++) {
			(*v)[i] = value[i];
		}

		_map.insert(std::pair<int, std::vector<double>*>(*key,v));
		_valSizes.insert(std::pair<int, unsigned int>(*key, valueSize));

	/****************************************/
	_nWtLock->lock();
	_numWriters--;
	_nWtLock->unlock();
	_writersLock->unlock();
	_readersLock->unlock();
	/****************************************/

}

void KeyValueMap::insert_meta(const keyvalue::meta& m)
{
	/****************************************/
	_readersLock->lock();
	while (_numReaders > 0) {}
	_writersLock->lock();
	while (_numReaders > 0) {}
	_nWtLock->lock();
	_numWriters++;
	_nWtLock->unlock();
	while (_numReaders > 0) {}
	/****************************************/
    
		int elems = m.value_size()/sizeof(double);
		std::vector<double> * v = new std::vector<double>(elems); //m.value(),m.value()+elems);
    
		_map_meta.insert(std::make_pair(m.key(),v));
		_valSizes_meta.insert(std::make_pair(m.key(),m.key_size()));

	/****************************************/
	_nWtLock->lock();
	_numWriters--;
	_nWtLock->unlock();
	_writersLock->unlock();
	_readersLock->unlock();
	/****************************************/

}



int KeyValueMap::retrieve(const int * key, unsigned int keySize, double * value, unsigned int valueSize, void * handle, bool async)
{
	unsigned int size = 0;

	/****************************************/
	_readersLock->lock();
	while (_numWriters > 0) {}
	_nRdLock->lock();
	_numReaders++;
	_nRdLock->unlock();
	_readersLock->unlock();
	/****************************************/


	std::multimap<int, std::vector<double> *>::iterator it = _map.find(*key);
	if (it != _map.end()) {

		//size = _valSizes[*key];

		std::vector<double> * v = it->second;
		size = v->size() * sizeof(double);

		if (valueSize < size) {
			std::cout
			<< "[" << _rank << "] Key-value retrieve FAILED: "
			<< " given value size too small, it should be " << size << " bytes" << std::endl;

			/****************************************/
			_nRdLock->lock();
			_numReaders--;
			_nRdLock->unlock();
			/****************************************/

			return 0;
		}

		for (unsigned int i = 0; i < v->size(); i++)
			value[i] = (*v)[i];

	} else {
		std::cout
		<< "[" << _rank << "] Key-value retrieve FAILED: "
		<< " key (" << (int) *key << ") does not exist" << std::endl;
	}
	/****************************************/
	_nRdLock->lock();
	_numReaders--;
	_nRdLock->unlock();
	/****************************************/
	return size;
}


void KeyValueMap::remove(const int * key, unsigned int keySize, void * handle, bool async)
{
	/****************************************/
	_readersLock->lock();
	while (_numReaders > 0) {}
	_writersLock->lock();
	while (_numReaders > 0) {}
	_nWtLock->lock();
	_numWriters++;
	_nWtLock->unlock();
	while (_numReaders > 0) {}
	/****************************************/

	std::multimap<int, std::vector<double> *>::iterator it = _map.find(*key);

	if (it != _map.end()) {
		delete it->second;
		_map.erase(it);
		_valSizes.erase(*key);
	} else {
		std::cout
		<< "[" << _rank << "] Key-value removal FAILED: "
		<< " key does not exist" << std::endl;

	}

	/****************************************/
	_nWtLock->lock();
	_numWriters--;
	_nWtLock->unlock();
	_writersLock->unlock();
	_readersLock->unlock();
	/****************************************/
}

