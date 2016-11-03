/*
 * ceph_store.cpp
 *
 */


#include <iostream>
#include <cstring>
#include <sstream>
#include <vector>
#include <cassert>

#include "keyvalue/ceph/ceph_store.h"
//#include "keyvalue/utils/tools.h"
#include "utils/mpi/controler.h"


keyvalue_ceph::keyvalue_ceph(ceph_uri uri):
        _async(false), _uri(uri) {
    _rank = mapp::master.rank();

    // client.vizpoc
    if (_uri._client == "") _uri._client = "client.vizpoc";
    const int init = _cluster.init2( _uri._client.c_str(), "ceph", 0 /*flags*/ );
    if( init < 0 ) {
        std::cout << "[" << _rank << "] Cannot initialize rados cluster: "
                << init << std::endl;
    }

    // /gpfs/bbp.cscs.ch/home/jplanas/.ceph/ceph.conf
    if (_uri._path == "") _uri._path = "/gpfs/bbp.cscs.ch/home/jplanas/.ceph/ceph.conf";
    const int conf = _cluster.conf_read_file(_uri._path.c_str());
    if( conf < 0 ) {
        std::cout << "[" << _rank << "] Cannot read ceph config '"
                << _uri._path << "': " << conf << std::endl;
    }

    const int conn = _cluster.connect();
    if( conn < 0 ) {
        std::cout << "[" << _rank << "] Could not connect rados cluster: "
                << conn << std::endl;
    }

    // vizpoc
    if (_uri._host == "") _uri._host = "vizpoc";
    const int ctx = _cluster.ioctx_create( _uri._host.c_str(), _context );
    if( ctx < 0 ) {
        std::cout << "[" << _rank << "] Could not create io context: "
                << ctx << std::endl;
    }
}


keyvalue_ceph::~keyvalue_ceph(){
    _context.close();
    _cluster.shutdown();
}

int keyvalue_ceph::retrieve(keyvalue::meta_ceph & m) {
    uint64_t size = 0;
    time_t time;
    const int stat = _context.stat( m.key(), &size, &time );
    if( stat < 0 || size == 0 )
    {
        std::cout << "[" << _rank << "] keyvalue retrieve FAILED: "
                << " key (" << m.key() << "), stat failed: "
                << ::strerror( -stat ) << std::endl;
        return 0;
    }

    assert(size == m.value_size());

    librados::bufferlist bl;
    const int read = _context.read( m.key(), bl, size, 0 );
    if( read < 0 ) {
        std::cout << "[" << _rank << "] keyvalue retrieve FAILED: "
                << " key (" << m.key() << "), read failed: "
                << ::strerror( -read ) << std::endl;
        return 0;
    }

    //std::copy(m.value(),m.value()+m.value_size(),v->begin());
    bl.copy( 0, bl.length(), (char *) m.value() );
    return size;
}



void keyvalue_ceph::remove(const keyvalue::meta_ceph& m){

    //if(!_async) { // sync remove
        const int remove = _context.remove( m.key() );
        if( remove < 0 ) {
            std::cout << "[" << _rank << "] keyvalue removal FAILED: "
                    << ::strerror( -remove ) << std::endl;
        }
    //}
}

void keyvalue_ceph::insert(keyvalue::meta_ceph& m){

    librados::bufferlist bl;
    bl.append((const char *) m.value(), m.value_size());

    if(_async) { // async write
        if (m.handle() == NULL) {
            std::cout << "Error: async operations need a non-NULL async I/O handle" << std::endl;
        }

        //std::cout << "[" << _rank << "] async Insert():: < " << *key << ", " << *value << " >" << std::endl;

        const int write = _context.aio_write_full( m.key(), m.handle(), bl );
        if( write < 0 ) {
            std::cout << "[" << _rank << "] Async keyvalue insertion FAILED: "
                    << ::strerror( -write ) << std::endl;
        } else {
            //std::cout
            //<< "[" << _rank << "] Async keyvalue pair insertion successful!"
            //<< std::endl;
        }
    } else {
        const int write = _context.write_full( m.key(), bl );
        if( write < 0 ) {
            std::cout << "[" << _rank << "] Keyvalue insertion FAILED: "
                    << ::strerror( -write ) << std::endl;
        }
    }
}

void keyvalue_ceph::wait(keyvalue::meta_ceph& m) {

    if (!_async) return;

    m.handle()->wait_for_complete();

    const int read = m.handle()->get_return_value();
    if( read < 0 ) {
        std::cerr <<  "[" << _rank << "] Wait FAILED: " << m.key()
                << ", " << ::strerror( -read ) << std::endl;
    } else {
        //std::cout
        //<< "[" << _rank << "] Wait successfully completed!"
        //<< std::endl;
    }
}
