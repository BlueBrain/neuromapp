/*
 * map_store.h
 *
 */

#ifndef MAP_STORE_H_
#define MAP_STORE_H_

#include <map>
#include <vector>

#include "utils/omp/lock.h"
//#include "keyvalue/lock.h"
#include "keyvalue/meta.h"

class keyvalue_map{

private:
    bool                                                _async;
    int                                                 _rank;
    std::multimap<std::string, std::vector<double> *>   _map;
    std::multimap<std::string, std::size_t>             _valSizes;

    // Allow concurrent reads, but exclusive writes
    // https://en.wikipedia.org/wiki/Readers%E2%80%93writers_problem#The_third_readers-writers_problem
    int                                             _numReaders;
    int                                             _numWriters;

#ifdef _OPENMP
    mapp::omp_lock _nRdLock,_nWtLock,_readersLock,_writersLock;
#else
    mapp::dummy_lock _nRdLock,_nWtLock,_readersLock,_writersLock;
#endif

public:
    explicit keyvalue_map (bool threadSafe = false, std::string pdsName = "");
    ~keyvalue_map();
    void insert (const keyvalue::meta& m);
    int retrieve (keyvalue::meta& m);
    void remove (const keyvalue::meta& m);
    void wait (const keyvalue::meta& m){}; //empty for the future if async;
    inline bool & async(){return _async;}
    inline bool const& async() const {return _async;}
};



#endif /* MAP_STORE_H_ */

