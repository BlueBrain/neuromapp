#include <iostream>
#include <algorithm>
#include <boost/hana.hpp>
#include <cassert>
#include <string>
namespace hana = boost::hana;

//names
struct Cat {
    //use special introspection tools
    BOOST_HANA_DEFINE_STRUCT(Cat,
    (std::string, name)
    );
};
struct Dog{ std::string name;};
struct Fish {std::string name;};

auto name_tup = hana::make_tuple (Cat{"garfield"},Dog{"fido"},Fish{"bruce"});

auto serial_cout = [](std::ostream & os,auto const &  ob) { 
    hana::for_each(hana::members(ob),[&] (auto ob_mem) {
            os << ob_mem <<  std::endl;
            });
};

int main() {
    using namespace hana::literals;
    Dog kitty = name_tup[1_c];// this is a literal compile time number trick 

    // no need for any type information to be used here, that's pretty nice
    hana::for_each(name_tup, [](auto a) {std::cout << " animal name is : " <<  a.name<< std::endl;});
    // versus the serial method here for one of tehm
    serial_cout(std::cout,name_tup[0_c]);

}



