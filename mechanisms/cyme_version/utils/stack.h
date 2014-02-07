
#ifndef COREBLURON_STACK_HPP
#define COREBLURON_STACK_HPP

namespace corebluron{

    class stack{
    public:
        typedef boost::function<void(void)> base_type; // pointer to function
        stack(){
           s.reserve(1024);
        }

        void push_back(base_type const& f){
            s.push_back(f); // fill up the stack
        }

        void flush(){
            // C - Pramon your #pragma here // OMP, CILK, OMPSS
            for(std::vector<base_type>::iterator it = s.begin(); it < s.end(); ++it)
                (*it)(); // execute cnrn_functions
        }
        private:
        std::vector<base_type> s;
    };
}//end namespace

#endif
