#pragma once

#include "coreneuron_1.0/common/memory/nrnthread.h"

#ifdef __cplusplus
    extern "C" {
        void nrn_solve_minimal(NrnThread* _nt);
        void triang(NrnThread*);
        void bksub(NrnThread*);
    }
#else
    void nrn_solve_minimal(NrnThread* _nt);
    void triang(NrnThread*);
    void bksub(NrnThread*);
#endif
