#ifndef COREBLURON_PACK_HPP
#define COREBLURON_PACK_HPP

namespace corebluron{

     // the mechanism should be independent from the type (float or double)
     // I use this trait to associate the basic type, and the size_type (number of element, still given into the mech)
     template<class mechanism, class T>
     struct trait_mechanism{
         typedef  T value_type;
         static const int value_size = mechanism::value_size;
     };
   
     // pack<mechanism::chanel::Na, double, memory::AoSoA>   
     template<class mechanism, class T,  memory::order O = memory::AoSoA, class container = cyme::vector<trait_mechanism<mechanism,T>, O> > // should use cyme container there
     class pack{
     public:
            typedef typename container::value_type value_type;
            typedef typename container::iterator iterator; 
        
            explicit pack(int size, int value):cont(size,value){ // vector constructor
            }
        
            explicit pack():cont(){ // array constructor
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
