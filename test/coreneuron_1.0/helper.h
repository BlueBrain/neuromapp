#pragma once

#include <string>

#include "utils/argv_data.h"
#include "test/coreneuron_1.0/path.h" // this file is generated automatically

namespace mapp{
    /** helper for the path of the input data */
    inline std::string data_test(){
        return mapp::helper_build_path::test_data_path()+"bench.101392/bench.101392";
    }

    /** helper for the path of the reference solution  */
    inline std::string data_ref(){
        return mapp::helper_build_path::test_data_path()+"rhs_d_ref/";
    }

    /** helper to convert a vector<string> to char * argv[] */
    inline char *convert(const std::string & s) {
        return strdup(s.c_str());
    }

    /** helper to execute the miniapp */
    template <typename C>
    int execute(const C &v, int(*ptf)(int,char * const *)) {
        argv_data A(v.begin(),v.end());
        return ptf(A.argc(),A.argv());
    }

    /** helper to compare to debug solution */
    void helper_check(std::string const& name, std::string const& mechanism, std::string const& path);
}
