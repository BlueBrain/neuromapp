#pragma once

#if defined(__OPENACC__)
    /* OpenACC accelerator  ---------------------------------------------- */
    // Putting it first as it's compiler independent (cray,pgi ...)
#define  PRAGMA_FOR_VECTOR_LOOP _Pragma("acc kernels")

#elif defined(__clang__)
    /* Clang/LLVM. ---------------------------------------------- */
#define PRAGMA_FOR_VECTOR_LOOP _Pragma("clang loop vectorize(enable)")

#elif defined(__ICC) || defined(__INTEL_COMPILER)
    /* Intel ICC/ICPC. ------------------------------------------ */
#define PRAGMA_FOR_VECTOR_LOOP _Pragma("ivdep")

#elif defined(__IBMC) || defined(__IBMCPP)
    /* IBM XL C/C++. -------------------------------------------- */
#define PRAGMA_FOR_VECTOR_LOOP _Pragma("ibm independent_loop")

#elif defined(__PGI)
    /* Portland Group PGCC/PGCPP. ------------------------------- */
#define PRAGMA_FOR_VECTOR_LOOP _Pragma("vector")

#elif defined(_CRAYC)
    /* CRAY. ----------------------------------- */
#define PRAGMA_FOR_VECTOR_LOOP _Pragma("_CRI ivdep")

#elif defined(__GNUC) || defined(__GNUG)
    /* GNU GCC/G++. --------------------------------------------- */
#define PRAGMA_FOR_VECTOR_LOOP _Pragma("GCC ivdep")

#else
    /* None of the above compiler? Fix this!! -------------------- */
#define PRAGMA_FOR_VECTOR_LOOP

#endif // PRAGMA_FOR_VECTOR_LOOP

