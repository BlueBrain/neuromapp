/*
 * Neuromapp - meta.h, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
 * All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/key-value/meta.h
 * \brief Implements the meta class to simplify key-value technology
 */

#ifndef META_H
#define META_H

#include <string>
#include <vector>

namespace keyvalue{

class meta{
public:
    typedef std::pair<void*,std::size_t> value_pair;

    /** \fn meta(std::string const& s, std::pair<void*,std::size_t> const& v_pair;)
	    \brief constructor of the meta information
	    \param s the key encapsulate the key value and its size
	    \param v_pair the value encapsulate the value pointer and the corresping size
    */
    explicit meta(std::string const& s = std::string(),
                   value_pair const& p = std::make_pair((void*)NULL,0)):k(s),vp(p){
    }

    /** \fn key()
	    \brief modify the key
     */
    inline std::string & key() {
        return k;
    }

    /** \fn key()
	    \brief read the key
     */
    inline std::string const & key() const {
        return k;
    }

    /** \fn key_size()
	    \brief return the key size in byte
     */
    inline std::size_t const key_size() const {
        return k.size();
    }

    /** \fn value() 
        \brief modify the pointer of the memory
     */
    inline void* & value() {
        return vp.first;
    }

    /** \fn value()
        \brief read the pointer of the memory
     */
    inline void* const & value() const {
        return vp.first;
    }

    /** \fn value_size()
        \brief modify the value of the size of the memory
     */
    inline std::size_t &value_size() {
        return vp.second;
    }

    /** \fn value_size()
        \brief read the value of the size of the memory
    */
    inline std::size_t const& value_size() const {
        return vp.second;
    }

private:
    /** tke key, the size of the key is a data memeber of the std::string */
    std::string k;
    /** the value, the pointer of the memory and  the associated size is stored in std::pair */
    value_pair vp;
};

//put ifdef in the futur
typedef int skv_client_cmd_ext_hdl_t; // for compilation

class meta_skv : public meta {
public:
    explicit meta_skv(std::string const& s = std::string(),
                      value_pair const& p = std::make_pair((void*)NULL,0)): meta(s,p) {
        rem_handles_ = skv_client_cmd_ext_hdl_t();
        ins_handles_ = skv_client_cmd_ext_hdl_t();
    }

    inline skv_client_cmd_ext_hdl_t const* rem_handles() const{
        return &rem_handles_;
    }

    inline skv_client_cmd_ext_hdl_t* rem_handles() {
        return &rem_handles_;
    }

    inline skv_client_cmd_ext_hdl_t const* ins_handles() const{
        return &ins_handles_;
    }

    inline skv_client_cmd_ext_hdl_t* ins_handles() {
        return &ins_handles_;
    }
        
    private:
        skv_client_cmd_ext_hdl_t rem_handles_;
        skv_client_cmd_ext_hdl_t ins_handles_;
    };

} //end namespace
#endif