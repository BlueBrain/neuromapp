/*
 * Neuromapp - lock.h, Copyright (c), 2015,
 * Kai Langen - Swiss Federal Institute of technology in Lausanne,
 * kai.langen@epfl.ch,
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
 * @file neuromapp/queueing/lock.h
 * Contains OMPLock and DummyLock class declaration.
 */

#ifndef MAPP_LOCK_H_
#define MAPP_LOCK_H_

#ifdef _OPENMP
	#include <omp.h>
#endif

class DummyLock{
public:
	DummyLock(){}
	~DummyLock(){}
	void acquire(){}
	void release(){}
};

class OMPLock{
private:
	omp_lock_t mut_;

public:
	/** \fn OMPLock()
	    \brief inits mut_
	 */
	OMPLock(){omp_init_lock(&mut_);}

	/** \fn ~OMPLock()
	    \brief destroys mut_
	 */
	~OMPLock(){omp_destroy_lock(&mut_);}

	/** \fn acquire()
	    \brief sets mut_
	 */
	inline void acquire(){omp_set_lock(&mut_);}

	/** \fn release()
	    \brief unsets mut_
	 */
	inline void release(){omp_unset_lock(&mut_);}
};
#endif
