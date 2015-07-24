
#include <cstdlib>
#include <string>
#include <boost/filesystem.hpp>

namespace mapp{
    /** helper for the path of the zip file */
    std::string path_zip(){
        return mapp::helper_src_path::path()+"/neuromapp/coreneuron_1.0/common/data/bench.101392.zip";
    }

    /** helper for the path of the unzip file */
    std::string path_unzip(){
        return mapp::helper_build_path::path()+"/neuromapp/coreneuron_1.0/test/bench.101392/bench.101392";
    }

    /** helper to convert a vector<string> to char * argv[] */
    char *convert(const std::string & s) {
        char *pc = new char[s.size()+1];
        std::strcpy(pc, s.c_str());
        return pc;
    }

    /** helper to execute the miniapp */
    int execute(std::vector<std::string> &v, int(*ptf)(int,char * const *)){
        std::vector<char*> command_vc;
        std::transform(v.begin(), v.end(), std::back_inserter(command_vc), mapp::convert);
        return ptf(command_vc.size(), &command_vc[0]);
    }
}
