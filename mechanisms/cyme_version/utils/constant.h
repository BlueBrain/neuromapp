
#ifndef COREBLURON_CONSTANT_HPP
#define COREBLURON_CONSTANT_HPP

namespace corebluron{

     template<class T>
     struct time{
         const static T dt() {return 0.01;} 
     };

}
#endif
