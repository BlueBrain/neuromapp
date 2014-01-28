#ifndef COREBLURON_PACK_HPP
#define COREBLURON_PACK_HPP

#include <boost/utility/enable_if.hpp>

namespace corebluron{
     // the mechanism should be independent from the type (float or double)
     // I use this trait to associate the basic type, and the size_type (number of element, still given into the mech)
     template<class mechanism, class T>
     struct trait_mechanism{
         typedef  T value_type;
         static const int value_size = mechanism::value_size;
     };

     /* SFINAE 
     The user wants a pack, this pack can be static or dynamic, same name but selection during compilation with SFINAE patern
     example: If I am looking for a dynamic one 
        pack<Na,double,AoS> a(129,0); I have a dynamic pack (vector), mechanism Na, double, memory order AoS, size 129, init value 0
        pack<Na,double,AoSoA,1238> a(1); I have a static pack (array), mechanism Na, double, memory order AoS, size 1238, init value 1
     */
     template<class mechanism, class T, memory::order O, int M, class Enable = void >
     struct selector_container;

     template<class mechanism, class T, memory::order O, int M>
     struct selector_container<mechanism, T, O, M,typename boost::enable_if_c<M == 0>::type>{
         typedef cyme::vector<trait_mechanism<mechanism,T>, O> c; // c for container
     };

     template<class mechanism, class T, memory::order O, int M>
     struct selector_container<mechanism, T, O, M,typename boost::enable_if_c<M != 0>::type>{
         typedef cyme::array<trait_mechanism<mechanism,T>, M, O> c; // c for container
     };

     template<class mechanism, class T,  memory::order O, int M=0, class container = typename selector_container<mechanism,T,O,M>::c > 
     class pack{
     public:
            typedef typename container::value_type value_type;
            typedef typename container::iterator iterator; 
        
            explicit pack(int size, int value):cont(size,value){ // vector constructor
            }
        
            explicit pack(int value):cont(value){ // array constructor
            }
        
            void execution(){
                for(typename container::iterator it = cont.begin(); it < cont.end(); ++it)
                    m.template cnrn_functions<typename container::iterator, container::order_value>(it);
            }
        
            value_type& operator()(size_t i, size_t j){
                return this->cont(i,j);
            }
        
            std::size_t size(){
                return this->cont.size();
            }
        
            std::size_t size_block(){
                return this->cont.size_block();
            }
        
            iterator begin(){
                return cont.begin();
            }

            iterator end(){
                return cont.end();
            }

        private:
            container cont;
            mechanism m;
     };
}
#endif
