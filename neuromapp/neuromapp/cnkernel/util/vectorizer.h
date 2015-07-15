#ifndef _nrn_vectorizer_
#define _nrn_vectorizer_

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

#endif // _nrn_vectorizer_
