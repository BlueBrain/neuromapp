#ifndef _nrnthread_
#define _nrnthread_

typedef struct Membrane {
    int nodecount;
    int szp;
    int szdp;
    int type;
    int is_art;
    long offset;

    int *pdata;
    double *data;
    int *nodeindices;
} Mechanism;

typedef struct NrnTh{
    int _ndata;
    int nmech;
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

    //@todo: add following extra members
    //nmech: temporarily stored, make struct member
} NrnThread;

#endif // _nrnthread_
