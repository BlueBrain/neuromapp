#include <vector>
#include "nest/synapse/models/tsodyks2.h"
#include "nest/synapse/scheduler.h"

namespace nest {
    //Initialize static member of class scheduler
    std::vector<node*> scheduler::nodes_vec_;
};
