#include <iostream>
#include <boost/hana.hpp>
#include <cassert>
#include <string>
namespace hana = boost::hana;

//names
struct Cat { std::string name;};
struct Dog{ std::string name;};
struct Fish {std::string name;};

auto name_tup = hana::make_tuple (Cat{"garfield"},Dog{"fido"},Fish{"bruce"});

int main() {
    using namespace hana::literals;
    Cat kitty = name_tup[1_c];
    std::cout << "cat name is "<<kitty.name << std::endl;
}



