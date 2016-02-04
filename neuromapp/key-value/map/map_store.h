/*
 * map_store.h
 *
 */

#ifndef MAP_STORE_H_
#define MAP_STORE_H_

#include <map>
#include <vector>

#include "key-value/lock.h"
#include "key-value/meta.h"

class keyvalue_map{

private:
    int											        _rank;
    std::multimap<std::string, std::vector<double> *>	_map;
    std::multimap<std::string, std::size_t>		    	_valSizes;


    // Allow concurrent reads, but exclusive writes
    // https://en.wikipedia.org/wiki/Readers%E2%80%93writers_problem#The_third_readers-writers_problem
    int							_numReaders;
    int							_numWriters;
    MyLock					  * _nRdLock;
    MyLock					  * _nWtLock;
    MyLock					  * _readersLock;
    MyLock					  * _writersLock;


public:
    explicit keyvalue_map (bool threadSafe = false, std::string pdsName = "");
    ~keyvalue_map();
    void insert (const keyvalue::meta& m);
    int retrieve (keyvalue::meta& m);
    void remove (const keyvalue::meta& m);
    void wait (const keyvalue::meta& m){}; //empty for the futur if async;
};



#endif /* MAP_STORE_H_ */

