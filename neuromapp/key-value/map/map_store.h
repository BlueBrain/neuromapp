/*
 * map_store.h
 *
 */

#ifndef MAP_STORE_H_
#define MAP_STORE_H_

#include <map>
#include <vector>

#include "key-value/kv-iface.h"
#include "key-value/lock.h"

#include "key-value/meta.h"


class KeyValueMap : public KeyValueIface {

private:
	int											_rank;
	std::multimap<int, std::vector<double> *>	_map;
	std::multimap<int, unsigned int>			_valSizes;

	std::multimap<std::string, std::vector<double> *>	_map_meta;
	std::multimap<std::string, std::size_t>			_valSizes_meta;


	// Allow concurrent reads, but exclusive writes
	// https://en.wikipedia.org/wiki/Readers%E2%80%93writers_problem#The_third_readers-writers_problem
	int							_numReaders;
	int							_numWriters;
	MyLock					  * _nRdLock;
	MyLock					  * _nWtLock;
	MyLock					  * _readersLock;
	MyLock					  * _writersLock;


public:
	KeyValueMap (int mpiRank = 0, bool threadSafe = false, std::string pdsName = "");
	virtual ~KeyValueMap();

	virtual void insert (const int * key, unsigned int keySize, const double * value, unsigned int valueSize, void * handle = NULL, bool async = false);
    void insert_meta(const keyvalue::meta& m);

	virtual int retrieve (const int * key, unsigned int keySize, double * value, unsigned int valueSize, void * handle = NULL, bool async = false);

	virtual void remove (const int * key, unsigned int keySize, void * handle = NULL, bool async = false);

};



#endif /* MAP_STORE_H_ */
