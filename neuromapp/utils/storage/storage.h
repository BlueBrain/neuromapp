#pragma once

typedef void *storage_ctor_context;
typedef void *(*storage_ctor)(storage_ctor_context);
typedef void (*storage_dtor)(void *);

/* C interface to storage represents stored items by void pointer,
 * and the functional constructor by a void * returning function that
 * takes a single void * context argument. */

/** C interface return void* to cast by the user */
void *storage_get(const char *name, storage_ctor maker,
                  storage_ctor_context context, storage_dtor destroyer );

/** C interface flush the memory */
void storage_put(const char *name, void *item, storage_dtor dtor);

void storage_clear(const char *name);

