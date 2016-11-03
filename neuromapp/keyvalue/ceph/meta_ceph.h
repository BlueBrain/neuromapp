
/*
 * Neuromapp - meta_ceph.h, Copyright (c), 2015,
 * Judit Planas - Swiss Federal Institute of technology in Lausanne,
 * judit.planas@epfl.ch,
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
 * @file neuromapp/keyvalue/ceph/meta_ceph.h
 * \brief Implements the meta class for Ceph - Rados only
 */

#ifndef META_CEPH_H
#define META_CEPH_H

#include <string>
#include <vector>

#include <rados/librados.hpp>

#include "keyvalue/meta.h"

namespace keyvalue{

class meta;

class meta_ceph : public meta {
public:
    typedef meta::value_type value_type;
    typedef value_type* pointer;
    typedef value_type const* const_pointer;

    /** \fn meta(std::string const& s, std::pair<double*,std::size_t> const& v_pair)
        \brief constructor of the meta information
        \param s the key encapsulate the key value and its size
        \param v_pair the value encapsulate the value pointer and the corresponding size
     */
    explicit meta_ceph(std::string const& s = std::string(),
                      value_pair const& p = std::make_pair((pointer)NULL,0)): meta(s,p) {
        h = librados::Rados::aio_create_completion();
    }

    /** \fn meta(std::string const& s, double* p, std::size_t n)
        \brief constructor of the meta information
        \param s the key encapsulate the key value and its size
        \param p the data
     \param n size of the data
     */
    explicit meta_ceph(std::string const& s = std::string(),
                      const_pointer p = (pointer) NULL,
                      std::size_t n = 0): meta(s,p,n) {
        h = librados::Rados::aio_create_completion();
    }

    /** \fun handle()
     \brief get the handle write only
     */
    inline librados::AioCompletion * handle() {
        return h;
    }

    /** \fun insert_reset()
     \brief reset insert handle
     */
    void reset() {
        h = librados::Rados::aio_create_completion();
    }

private:
    /** Async I/O completion handle */
    librados::AioCompletion * h;
};

}
#endif
