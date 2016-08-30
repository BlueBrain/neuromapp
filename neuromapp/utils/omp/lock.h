/*
 * Neuromapp - lock.h, Copyright (c), 2015,
 * Kai Langen - Swiss Federal Institute of technology in Lausanne,
 * kai.langen@epfl.ch,
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
 * @file neuromapp/utils/omp/lock.h
 * \brief Contains omp_lock and dummy_lock class declaration.
 */

#ifndef MAPP_LOCK_H_
#define MAPP_LOCK_H_

#ifdef _OPENMP
#include <omp.h>

namespace mapp {

class omp_mutex{
private:
	omp_lock_t mut_;

public:
	/** \fn omp_lock()
	 *  \brief inits mut_
	 */
	omp_mutex(){omp_init_lock(&mut_);}

	/** \fn ~omp_lock()
	 *  \brief destroys mut_
	 */
	~omp_mutex(){omp_destroy_lock(&mut_);}

	/** \fn acquire()
	 *  \brief sets mut_
	 */
	inline void lock(){omp_set_lock(&mut_);}

	/** \fn release()
	 *  \brief unsets mut_
	 */
	inline void unlock(){omp_unset_lock(&mut_);}
};

    typedef omp_mutex mutex ;
}

#else

class dummy_mutex{
public:
	void lock(){}
	void unlock(){}
};

    typedef dummy_mutex mutex;
}
#endif

#endif
