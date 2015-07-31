#ifndef _nrnthread_
#define _nrnthread_

/**
 * Represents the neuron membrane channels (in a compartment)
 */
typedef struct Membrane {
    int nodecount; // # of mechanism instances
    int szp; // number of (double) needed per instance of mechanism (e.g. Na=,synapse 35)
    int szdp;
    int type;
    int is_art;
    long offset;

    int *pdata;
    double *data;
    int *nodeindices;
} Mechanism;

/**
 * A dataset representing a group of cells, their compartments, mechanisms, etc.
 */
typedef struct NrnTh{
    int _ndata;
    int nmech;
    int ncell;
    int end;   //this is number of compartments
    double dt;

    double *_data;
    double *_actual_rhs;
    double *_actual_d;
    double *_actual_a;
    double *_actual_b;
    double *_actual_v;
    double *_actual_area;
    double *_shadow_rhs;
    double *_shadow_d;
    long max_nodecount;

    Mechanism *ml;

    int* _v_parent_index;

} NrnThread;

#endif // _nrnthread_
