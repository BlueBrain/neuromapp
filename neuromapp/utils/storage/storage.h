/*
 * Neuromapp - storage.h, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * Cremonesi Francesco - Swiss Federal Institute of technology in Lausanne,
 * Sam Yates - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
 * francesco.cremonesi@epfl.ch
 * sam.yates@epfl.ch
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
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/utils/storage/storage.h
 * \brief implementation of the storage class
 */

#ifndef MAPP_STORAGE_
#define MAPP_STORAGE_

typedef void *storage_ctor_context;
typedef void *(*storage_ctor)(storage_ctor_context);
typedef void (*storage_dtor)(void *);

/* C interface to storage represents stored items by void pointer,
 * and the functional constructor by a void * returning function that
 * takes a single void * context argument. */

#ifdef __cplusplus
extern "C" {
#endif

/** C interface return void* to cast by the user */
void *storage_get(const char *name, storage_ctor maker,
                  storage_ctor_context context, storage_dtor destroyer );

/** C interface flush the memory */
void storage_put(const char *name, void *item, storage_dtor dtor);

/** clearing the memory */
void storage_clear(const char *name);


#ifdef __cplusplus
}
#endif

#endif
