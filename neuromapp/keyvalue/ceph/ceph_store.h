/*
 * ceph_store.h
 *
 */

#ifndef CEPH_STORE_H_
#define CEPH_STORE_H_

#include <map>
#include <vector>

#include <rados/librados.hpp>

#include "utils/omp/lock.h"
#include "keyvalue/ceph/meta_ceph.h"

class keyvalue_ceph{

public:
    struct ceph_uri {
        std::string _client;
        std::string _host;
        std::string _path;
    };

private:
    bool                    _async;
    int                     _rank;
    ceph_uri                _uri;
    librados::Rados         _cluster;
    librados::IoCtx         _context;


public:
    explicit keyvalue_ceph (ceph_uri uri = ceph_uri());
    ~keyvalue_ceph();
    void insert (keyvalue::meta_ceph & m);
    int retrieve (keyvalue::meta_ceph & m);
    void remove (const keyvalue::meta_ceph & m);
    void wait (keyvalue::meta_ceph & m);
    inline bool & async(){return _async;}
    inline bool const & async() const {return _async;}
};



#endif /* CEPH_STORE_H_ */

