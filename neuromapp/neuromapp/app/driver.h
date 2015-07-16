#pragma once

#include <iostream>
#include <string>
#include <map>

namespace mapp{

    int usage(){
        std::cout << "Usage: < >  means optionnal \n";
        std::cout << "   miniapp --help provide the help of your miniapp \n \n";
        std::cout << "   The list of the following miniapps are available: \n";
        std::cout << "       hello <arg> \n";
        std::cout << "       kernel <arg> \n";
        std::cout << "       solver <arg> \n";
        return 0;
    }

    /** Driver for the execution of the miniapps

    The driver encapsulates functional of all miniapp, the key is the name
    of the miniapp, the use should provide the a function, XXXX_execute function respecting
    the tiny API.

        int XXXX_execute(int argc, char* argv);

    The user is responsible of the helper

    \remark the name is optional nevertheless all mini-apps must have differents name for the execution
    function, we suggest NAMEMINIAPP_execute(int argc, char* argv)
    */
    class driver{
        public:

        void insert(const std::string name, int(*f)(int,char **) ){
            m.insert(std::pair<std::string, int(*)(int,char **)>(name,f));
        }

        void execute(int argc, char * argv[]){
            if(argc == 1)
                usage();
            else{
                std::map<std::string, int(*)(int,char **)>::const_iterator it;
                it = m.find(std::string(argv[1]));
                (it == m.end()) ? usage() : it->second(argc-1, &argv[1]);
            }
        }

        private:
        std::map<std::string, int(*)(int,char **) > m;
    };

}// end namespace
