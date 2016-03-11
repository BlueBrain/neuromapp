#ifndef pool_adapter_h
#define pool_adapter_h

#include "coreneuron_1.0/queueing/pool.h"

namespace queueing{

/** creates an adapter for the queueing::pool<mutex> class
 * who's constructor matches that of spike::environment
 * (int eventsPer, int numOut, int numIn, int size, int rank)
 *
 * This allows easy templatized testing of functions
 * relating to spike exchange.
 *
 * Other parameters are set to default values:
 * cell groups = 64
 * percent ite = 0
 * percent spike = 100
 * verbose = false
 * with-algebra = false
 *
 * These arguments were chosen for fast testing,
 * and test cases with deterministic results
 */
class pool_adapter : public pool {
public:
    pool_adapter(int nSpike, int simtime, int numOut,
        int numIn, int netcons, int size, int rank):
        pool(64, 0, 0, simtime, true, false, nSpike,
            numOut, numIn, netcons, size, rank){};
};

}

#endif
