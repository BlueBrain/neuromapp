#ifndef CXX_DEMANGLE_H
#define CXX_DEMANGLE_H

#include <string>
#include <typeinfo>

std::string cxx_demangle(const char *mangled);

template <typename T>
std::string type_name() { return cxx_demangle(typeid(T).name()); }

#endif // ndef  CXX_DEMANGLE_H

