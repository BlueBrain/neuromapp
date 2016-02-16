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
class pool_adapter : public pool<mutex> {
public:
    pool_adapter(int eventsPer, int numOut, int numIn, int size, int rank):
        pool<mutex>(64, eventsPer, 0, false, false, 100, numOut, numIn, size, rank){};
};

}

#endif
