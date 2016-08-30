#include <vector>
#include "nest/models/tsodyks2.h"
#include "nest/nestkernel/environment/scheduler.h"

namespace nest {
    //Initialize static member of class scheduler
    std::vector<node*> scheduler::nodes_vec_;
};
