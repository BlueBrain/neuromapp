#pragma once

#include <string>
#include <vector>

#include "coreneuron_1.0/test/path.h" // this file is generated automatically

namespace mapp{
    /** helper for the path of the unzip file */
    inline std::string path_unzip(){
        return mapp::helper_build_path::path()+"/neuromapp/coreneuron_1.0/test/bench.101392/bench.101392";
    }

    /** helper to convert a vector<string> to char * argv[] */
    inline char *convert(const std::string & s) {
        return strdup(s.c_str());
    }

    /** helper to execute the miniapp */
    int execute(std::vector<std::string> &v, int(*ptf)(int,char * const *));

    /** helper to compare to debug solution */
    void helper_check(std::string const& name, std::string const& mechanism, std::string const& path);
}
