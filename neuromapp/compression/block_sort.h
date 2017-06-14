#include <iostream>
#include <string>
#include <iostream>
#include <string>
#include <algorithm>
#include "block.h"
//TODO remove std namespace
//

using namespace std;

namespace neuromapp {
    // create standard block sort,
    // and specialized versions for 1 dim and 2 dims
    class Sorter {
        public:
            template <typename value_type>
            static bool sort_rule(const value_type & rhs, const value_type & lhs) {return rhs > lhs;}
    };
}

