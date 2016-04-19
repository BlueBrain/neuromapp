
/*
 * Neuromapp - meta_skv.h, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details..  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/keyvalue/skv/meta_skv.h
 * \brief Implements the meta class for skv IBM only
 */

#ifndef META_SKV_H
#define META_SKV_H

#include <string>
#include <vector>

#include <skv/client/skv_client.hpp>


#ifdef SKV_STORE
#include "keyvalue/meta.h"
#endif



namespace keyvalue{

class meta;

class meta_skv : public meta {
public:
    typedef meta::value_type value_type;
    typedef value_type* pointer;
    typedef value_type const* const_pointer;

    /** \fn meta(std::string const& s, std::pair<double*,std::size_t> const& v_pair)
	    \brief constructor of the meta information
	    \param s the key encapsulate the key value and its size
	    \param v_pair the value encapsulate the value pointer and the corresping size
     */
    explicit meta_skv(std::string const& s = std::string(),
                      value_pair const& p = std::make_pair((pointer)NULL,0)): meta(s,p) {
        h = skv_client_cmd_ext_hdl_t();
    }

    /** \fn meta(std::string const& s, double* p, std::size_t n)
	    \brief constructor of the meta information
	    \param s the key encapsulate the key value and its size
	    \param p the data
     \param n size of the data
     */
    explicit meta_skv(std::string const& s = std::string(),
                      const_pointer p = (pointer) NULL,
                      std::size_t n = 0): meta(s,p,n) {
        h = skv_client_cmd_ext_hdl_t();
    }

    /** \fun ins_handles() const
     \brief get the handle read only
     */
    inline skv_client_cmd_ext_hdl_t const* handle() const{
        return &h;
    }

    /** \fun ins_handles()
     \brief get the handle write only
     */
    inline skv_client_cmd_ext_hdl_t* handle() {
        return &h;
    }

    /** \fun insert_reset()
     \brief reset insert handle
     */
    void reset(){
        h = skv_client_cmd_ext_hdl_t();
    }

private:
    /** handle */
    skv_client_cmd_ext_hdl_t h;
};

}

#endif

