
#include <cstdlib>
#include <string>
#include <boost/filesystem.hpp>

namespace mapp{
    /** helper for the path of the unzip file */
    std::string path_unzip(){
        return mapp::helper_build_path::path()+"/neuromapp/coreneuron_1.0/test/bench.101392/bench.101392";
    }

    /** helper to convert a vector<string> to char * argv[] */
    char *convert(const std::string & s) {
        return strdup(s.c_str());
    }

    /** helper to execute the miniapp */
    int execute(std::vector<std::string> &v, int(*ptf)(int,char * const *)){
        std::vector<char*> command_vc;
        std::transform(v.begin(), v.end(), std::back_inserter(command_vc), mapp::convert);
        return ptf(command_vc.size(), &command_vc[0]);
    }
}
