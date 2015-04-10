#include <string>
#include <cstdlib>

#if defined(__GXX_ABI_VERSION)
#include <cxxabi.h>

std::string cxx_demangle(const char *s) {
    int status=0;
    char *d=abi::__cxa_demangle(s,0,0,&status);

    if (status) {
        free(d);
        return s;
    }

    std::string r(d);
    std::free(d);
    return r;
}

#else

std::string cxx_demangle(const char *s) { return s; }

#endif 

