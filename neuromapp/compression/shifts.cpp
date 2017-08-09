#include <iostream>
#include <math>
#include <algorithm>
#include <vector>
#include <cstdint>
#include <cstdlib>
using namespace std;

struct insert_minder{ 
    int v_i;    
    int pos;
    int shift_type;
    int rel_start;// relative starting position
    public:
    insert_minder(int rel_start_arg,int shift_type_arg) :rel_start {rel_start_arg},shift_type{shift_type_arg} {
        v_i = floor(rel_start/32);// tells us which element in storage vector we start adding into
        pos = rel_start%32;// what position in that vector element to add 
    }

    void add_in(vector<uint32_t> vct, uint32_t val ) {
       // for (int i = 0;i < 32-pos; i++) {
       //  
       // }
}


union conv_bits{
    float val;
    uint32_t bits;
} ;

uint32_t get_sign(conv_bits cb) {
    uint32_t shift = 31;
    return cb.bits >> shift; 
}

uint32_t get_exp(conv_bits cb) {
    uint32_t mask = 255;
    uint32_t shift = 23;
    return (cb.bits >> shift) & mask; 
}


uint32_t get_mant(conv_bits cb) {
    uint32_t mask = 16777214;
    return cb.bits & mask; 
}

// could use closure? but question is how 



int main () {
    vector<float> vct{8.0,-9.0,10.0,-11.0,12.0};
    // minders would be used for helping to shift number insertion to next frame, and position correctly
    vector<uint32_t> store(5);

    std::fill(store.begin(),store.end(),0);
    conv_bits cb;
    for (float ele:  vct) {
        cb.val = ele;
        uint32_t sign = get_sign(cb);
        uint32_t exp = get_exp(cb);
        uint32_t mant = get_mant(cb);
    }

}


