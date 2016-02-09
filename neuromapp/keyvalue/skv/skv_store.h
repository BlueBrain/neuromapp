/*
 * skv_store.h
 *
 */

//#ifdef SKV_STORE

#ifndef SKV_STORE_H_
#define SKV_STORE_H_

#include <string>
#include <skv/client/skv_client.hpp>

#include "keyvalue/lock.h"
#include "keyvalue/meta.h"

class keyvalue_skv{

	typedef enum { SKV_NONE, SKV_INIT, SKV_CONNECT, SKV_PDS } SKV_STATE;

private:
    bool        _async;
    int	        _rank;
	SKV_STATE	_skvState;
	skv_client_t	_skvClient;
	skv_pds_id_t	_pdsId;

	MyLock			* _lock;

public:
    explicit keyvalue_skv (bool threadSafe = false, std::string pdsName = "");
    ~keyvalue_skv();
    void insert (const keyvalue::meta_skv& m);
    int retrieve (keyvalue::meta_skv& m);
    void remove (const keyvalue::meta_skv& m);
    void wait (const keyvalue::meta_skv& m);
    inline bool & async(){return _async;}
    inline bool const& async() const {return _async;}
};


#endif /* SKV_STORE_H_ */


