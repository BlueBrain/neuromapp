/*
 * Neuromapp - pool.h, Copyright (c), 2015,
 * Judit Planas - Swiss Federal Institute of technology in Lausanne,
 * judit.planas@epfl.ch,
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
 * @file neuromapp/key-value/kv-iface.h
 * contains declaration for the KeyValueIface interface class
 */


#ifndef kviface_h
#define kviface_h

#include <string>

class KeyValueIface {

public:
	KeyValueIface (int mpiRank = 0, bool threadSafe = false, std::string pdsName = "") {}
	virtual ~KeyValueIface() {}

	//virtual void insert(bool async, char * key, char * value, void * handle = NULL) {}

	//template<typename KT, typename VT>
	virtual void insert (const int * key, unsigned int keySize, const double * value, unsigned int valueSize, void * handle = NULL, bool async = false) = 0;

	virtual int retrieve (const int * key, unsigned int keySize, double * value, unsigned int valueSize, void * handle = NULL, bool async = false) = 0;

	virtual void remove (const int * key, unsigned int keySize, void * handle = NULL, bool async = false) = 0;

	//template<typename KT>
	//void remove(bool async, const KT * key, unsigned int keySize, void * handle = NULL) {}

	virtual void wait (void * handle) {}

};

#endif /* kviface_h */
