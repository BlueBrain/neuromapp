/*
 * Neuromapp - error.h, Copyright (c), 2015,
 * timothee ewart - Swiss Federal Institute of technology in Lausanne,
 * Sam Yates - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch
 * sam.yates@epfl.ch
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
 * @file neuromapp/utils/error.h
 *  devine the error for C/C++ interface
 */


#ifndef MAPP_ERROR_
#define MAPP_ERROR_

#ifdef __cplusplus
namespace mapp{
#endif

enum mapp_error {
    MAPP_OK =0,
    MAPP_BAD_ARG,
    MAPP_USAGE,
    MAPP_BAD_DATA,
    MAPP_BAD_THREAD,
    MAPP_UNKNOWN_ERROR
};

#ifdef __cplusplus
} // end namespace
#endif

#endif // ndef MAPP_ERROR_
