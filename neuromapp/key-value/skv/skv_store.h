/*
 * skv_store.h
 *
 */

//#ifdef SKV_STORE

#ifndef SKV_STORE_H_
#define SKV_STORE_H_


#include <string>


#include "key-value/kv-iface.h"
#include "key-value/lock.h"


#include <skv/client/skv_client.hpp>
#includef "key-value/meta.h"



class KeyValueSkv : public KeyValueIface {

	typedef enum { SKV_NONE, SKV_INIT, SKV_CONNECT, SKV_PDS } SKV_STATE;

private:
	SKV_STATE		_skvState;
	skv_client_t	_skvClient;
	skv_pds_id_t	_pdsId;

	MyLock			* _lock;

public:
	KeyValueSkv (int mpiRank = 0, bool threadSafe = false, std::string pdsName = "");
	virtual ~KeyValueSkv();

	virtual void insert (const int * key, unsigned int keySize, const double * value, unsigned int valueSize, void * handle = NULL, bool async = false);

	virtual int retrieve (const int * key, unsigned int keySize, double * value, unsigned int valueSize, void * handle = NULL, bool async = false);

	virtual void remove (const int * key, unsigned int keySize, void * handle = NULL, bool async = false);

	void wait (void * handle);

};


#endif /* SKV_STORE_H_ */

//#endif /* SKV_STORE */

