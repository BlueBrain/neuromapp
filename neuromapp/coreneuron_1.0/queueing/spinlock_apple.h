/*
 * Neuromapp - spinlock_apple.h, Copyright (c), 2015,
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
 * @file neuromapp/coreneuron_1.0/queueing/spinlock_apple.h
 * \brief missing spinlock implementation for mac
 */

#ifndef MAPP_SPINLOCK_APPLE_
#define MAPP_SPINLOCK_APPLE_

#include <errno.h>
#include <libkern/OSAtomic.h>

namespace queueing {

typedef OSSpinLock pthread_spinlock_t;

/** \fn pthread_spin_init(pthread_spinlock_t *lock, int pshared)
 wrapper for pthread_spin_init
 */
static inline int pthread_spin_init(pthread_spinlock_t *lock, int pshared) {
    OSSpinLockUnlock(lock);
    return 0;
}

/** \fn pthread_spin_destroy(pthread_spinlock_t *lock)
Destroy the spinlock nothing special
 */
static inline int pthread_spin_destroy(pthread_spinlock_t *lock) {
    return 0;
}

/** \fn pthread_spin_lock(pthread_spinlock_t *lock)
calling the MAC specific function
 */
static inline int pthread_spin_lock(pthread_spinlock_t *lock) {
    OSSpinLockLock(lock);
    return 0;
}

/** \fn pthread_spin_trylock(pthread_spinlock_t *lock)
 calling the MAC specific function
 */
static inline int pthread_spin_trylock(pthread_spinlock_t *lock) {
    if (OSSpinLockTry(lock)) {
        return 0;
    }
    return EBUSY;
}

/** \fn pthread_spin_unlock(pthread_spinlock_t *lock)
 calling the MAC specific function
 */
static inline int pthread_spin_unlock(pthread_spinlock_t *lock) {
    OSSpinLockUnlock(lock);
    return 0;
}

}
#endif
