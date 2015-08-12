/* Neuromapp - vectorizer.h, Copyright (c), 2015,
* Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
* Pramod Kumbhar - Swiss Federal Institute of technology in Lausanne,
* timothee.ewart@epfl.ch,
* paramod.kumbhar@epfl.ch
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
 * @file neuromapp/coreneuron_1.0/common/util/vectorizer.h
 * provide macro for the vectorization
 */
#ifndef MAPP_VECTORIZER_
#define MAPP_VECTORIZER_

/* With OpenACC ---------------------------- */
#if defined(_OPENACC)

#define _PRAGMA_FOR_ACC_LOOP_ _Pragma("acc parallel loop")

#endif //_OPENACC

#if defined(__clang__)
    /* Clang/LLVM. ---------------------------------------------- */
#define _PRAGMA_FOR_VECTOR_LOOP_ _Pragma("clang loop vectorize(enable)")

#elif defined(__ICC) || defined(__INTEL_COMPILER)
    /* Intel ICC/ICPC. ------------------------------------------ */
#define _PRAGMA_FOR_VECTOR_LOOP_ _Pragma("ivdep")

#elif defined(__IBMC__) || defined(__IBMCPP__)
    /* IBM XL C/C++. -------------------------------------------- */
#define _PRAGMA_FOR_VECTOR_LOOP_ _Pragma("ibm independent_loop")

#elif defined(__PGI)
    /* Portland Group PGCC/PGCPP. ------------------------------- */
#define _PRAGMA_FOR_VECTOR_LOOP_ _Pragma("vector")

#elif defined(_CRAYC)
    /* CRAY. ----------------------------------- */
#define _PRAGMA_FOR_VECTOR_LOOP_ _Pragma("_CRI ivdep")

#elif defined(__GNUC__) || defined(__GNUG__)
    /* GNU GCC/G++. --------------------------------------------- */
#define _PRAGMA_FOR_VECTOR_LOOP_ _Pragma("GCC ivdep")

#else
    /* None of the above compiler? Fix this!! -------------------- */
#define _PRAGMA_FOR_VECTOR_LOOP_

#endif // _PRAGMA_FOR_VECTOR_LOOP_

#endif
