#include <iostream>
#include <iterator>
#include <cmath>
#include <algorithm>
#include <vector>
#include <cstdint>
#include <cstdlib>
using namespace std;

struct insert_minder{ 
    int v_i;    
    int pos;
    int rel_start;// relative starting position
    int shift_type;
    public:
    insert_minder(int rel_start_arg,int shift_type_arg) :rel_start {rel_start_arg},shift_type{shift_type_arg} {
        v_i = floor(rel_start/32);// tells us which element in storage vector we start adding into
        pos = 31-rel_start%32;// what position in that vector element to add 
    }

    void add_in(vector<uint32_t> & vct, uint32_t val ) {
        for (int i = 1; i <= shift_type;i++) {
            uint32_t frame_val = (val >> (shift_type -i)) & 1 ;
            std::cout << "fv: " << frame_val << std::endl;
            vct[v_i] += frame_val << pos ;
            pos--;
            if (pos < 0) {// move to the start of the next number
                v_i++;
                pos = 31;
            }
        }
    }
};


union conv_bits{
    float val;
    uint32_t bits;
} ;

uint32_t get_sign(conv_bits cb) {
    uint32_t mask = 1;
    uint32_t shift = 31;
    return (cb.bits >> shift) & mask  ; 
}

uint32_t get_exp(conv_bits cb) {
    uint32_t mask = 0xff;
    uint32_t shift = 23;
    return (cb.bits >> shift) &  mask ; 
}


uint32_t get_mant(conv_bits cb) {
    uint32_t mask = 0xfffffe;
    return cb.bits & mask; 
}




int main () {
    vector<float> vct{-9.0};
    int N = vct.size();//magic
    // minders would be used for helping to shift number insertion to next frame, and position correctly
    // TODO remove magic numbers
    vector<uint32_t> store(N);

    std::fill(store.begin(),store.end(),0);
    insert_minder sign_inserter(0,1);//magic
    insert_minder exp_inserter(N,8);//magic
    insert_minder mant_inserter(8*N+N,23);//magic
    conv_bits cb;
    std::cout << "---" << std::endl;
    std::copy(store.begin(),store.end(),std::ostream_iterator<unsigned int>(std::cout, " "));
    std::cout << "---" << std::endl;
    for (float ele:  vct) {
        cb.val = ele;
        uint32_t sign = get_sign(cb);
        sign_inserter.add_in(store,sign);
        uint32_t exp = get_exp(cb);
        exp_inserter.add_in(store,exp);
        uint32_t mant = get_mant(cb);
        mant_inserter.add_in(store,mant);
    }
    std::cout << "---" << std::endl;
    std::copy(store.begin(),store.end(),std::ostream_iterator<unsigned int>(std::cout, " "));
    std::cout << "---" << std::endl;


}


