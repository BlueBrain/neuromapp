/*
 * map_store.cpp
 *
 */


#include <iostream>
#include <cstring>
#include <sstream>
#include <vector>
#include <cassert>

#include "keyvalue/map/map_store.h"
#include "keyvalue/utils/tools.h"
#include "utils/mpi/controler.h"


keyvalue_map::keyvalue_map(bool threadSafe, std::string pdsName):
        _numReaders(0), _async(false), _numWriters(0){
    _rank = mapp::master.rank();
}


keyvalue_map::~keyvalue_map(){
    _map.clear();
    _valSizes.clear();
}

int keyvalue_map::retrieve(keyvalue::meta& m)
{
    unsigned int size = 0;

    /****************************************/
    _readersLock.acquire();
    while (_numWriters > 0) {}
    _nRdLock.acquire();
    _numReaders++;
    _nRdLock.release();
    _readersLock.release();
    /****************************************/


    std::multimap<std::string, std::vector<double> *>::iterator it = _map.find(m.key());
    if (it != _map.end()) {

        std::vector<double> * v = it->second;
        size = v->size() * sizeof(double);

        assert(v->size() == m.value_size());

        if (m.value_size() < size) {
            std::cout
            << "[" << _rank << "] keyvalue retrieve FAILED: "
            << " given value size too small, it should be " << size << " bytes" << std::endl;

            /****************************************/
            _nRdLock.acquire();
            _numReaders--;
            _nRdLock.release();
            /****************************************/

            return 0;
        }

        std::copy(m.value(),m.value()+m.value_size(),v->begin());

    } else {
        std::cout
        << "[" << _rank << "] keyvalue retrieve FAILED: "
        << " key (" << m.key() << ") does not exist" << std::endl;
    }
    /****************************************/
    _nRdLock.acquire();
    _numReaders--;
    _nRdLock.release();
    /****************************************/
    return size;
}



void keyvalue_map::remove(const keyvalue::meta& m){
    /****************************************/
    _readersLock.acquire();
    while (_numReaders > 0) {}
    _writersLock.acquire();
    while (_numReaders > 0) {}
    _nWtLock.acquire();
    _numWriters++;
    _nWtLock.release();
    while (_numReaders > 0) {}
    /****************************************/

    std::multimap<std::string, std::vector<double> *>::iterator it = _map.find(m.key());

    if (it != _map.end()) {
        delete it->second;
        _map.erase(it);
        _valSizes.erase(m.key());
    } else {
        std::cout
        << "[" << _rank << "] keyvalue removal FAILED: "
        << " key does not exist" << std::endl;

    }

    /****************************************/
    _nWtLock.acquire();
    _numWriters--;
    _nWtLock.release();
    _writersLock.release();
    _readersLock.release();
    /****************************************/
}

void  keyvalue_map::insert(const keyvalue::meta& m){
    /****************************************/
    _readersLock.acquire();
    while (_numReaders > 0) {}
    _writersLock.acquire();
    while (_numReaders > 0) {}
    _nWtLock.acquire();
    _numWriters++;
    _nWtLock.release();
    while (_numReaders > 0) {}
    /****************************************/
    std::vector<double> * v = new std::vector<double>(m.value(),m.value()+m.value_size());

    _map.insert(std::make_pair(m.key(),v));
    _valSizes.insert(std::make_pair(m.key(),m.key_size()));

    /****************************************/
    _nWtLock.acquire();
    _numWriters--;
    _nWtLock.release();
    _writersLock.release();
    _readersLock.release();
    /****************************************/
    
}

