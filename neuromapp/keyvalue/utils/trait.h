/*
 * Neuromapp - trait.h, Copyright (c), 2015,
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
 * GNU General Public License for more details. See
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/keyvalue/utils/trait.h
 * \brief trait for the backend in function of the meta
 */

#ifndef MAPP_TRAIT_H
#define MAPP_TRAIT_H

#include "keyvalue/map/map_store.h"
#ifdef SKV_STORE
#include "keyvalue/skv/skv_store.h"
#endif
#include "keyvalue/meta.h"

namespace keyvalue {

    enum selector { map = 0, skv = 1};

    template<selector M>
    struct trait_meta;

    template<>
    struct trait_meta<map>{
        typedef meta meta_type;
        typedef keyvalue_map keyvalue_type;
    };

    template<>
    struct trait_meta<skv>{
#ifdef SKV_STORE
        typedef meta_skv meta_type;
        typedef keyvalue_skv keyvalue_type;
#else
        typedef meta meta_type;
        typedef keyvalue_map keyvalue_type;
#endif
    };

} //end namespace keyvalue

#endif
