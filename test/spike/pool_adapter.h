#ifndef pool_adapter_h
#define pool_adapter_h

#include "coreneuron_1.0/queueing/pool.h"

namespace queueing{

class pool_adapter : public pool<mutex> {
public:
    /* create a default pool environment that uses:
     * mutex
     * 64 cell groups
     * verbose = false
     * with-algebra = true
     */
    pool_adapter(int eventsPer, int numOut, int numIn, int size, int rank):
        pool(64, eventsPer, 0, false, false, 100, numOut, numIn, size, rank){};
};

}

#endif
