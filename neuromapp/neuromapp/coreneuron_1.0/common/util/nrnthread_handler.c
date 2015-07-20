#include "coreneuron_1.0/common/util/nrnthread_handler.h"

void * make_nrnthread(void * context) {
    const char * filename = (const char*)context;
    void * nt = malloc(sizeof(NrnThread));
    read_nt_from_file(filename, (NrnThread*)nt);
    return nt;
}

void dealloc_nrnthread(void * p) {

    NrnThread * nt = (NrnThread * )p;

    free(nt->_shadow_d);
    nt->_shadow_d = NULL;
    free(nt->_shadow_rhs);
    nt->_shadow_rhs = NULL;
    free(nt->_v_parent_index);
    nt->_v_parent_index = NULL;

    int i;
    for (i=nt->nmech-1; i>=0; i--) {
        Mechanism *ml = &nt->ml[i];
        free(ml->pdata);
        ml->pdata = NULL;
        free(ml->nodeindices);
        ml->nodeindices = NULL;
    }

    free(nt->ml);
    nt->ml = NULL;
    free(nt->_data);
    nt->_data = NULL;

    nt = NULL;

}

