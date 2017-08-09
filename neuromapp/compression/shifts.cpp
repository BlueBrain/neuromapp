#include <iostream>
#include <iterator>
#include <cmath>
#include <algorithm>
#include <vector>
#include <cstdint>
#include <cstdlib>
using namespace std;

template <typename T>
struct insert_minder {};

template <>
struct insert_minder<float>{ 
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
            std::cout << frame_val;
            vct[v_i] += frame_val << pos ;
            pos--;
            if (pos < 0) {// move to the start of the next number
                v_i++;
                pos = 31;
            }
        }
    }
};

template <typename T>
struct cbit_holder {};

template <>
struct cbit_holder<float> {
    union conv_bits{
        float val;
        uint32_t bits;
    };
};


template <>
struct cbit_holder<double> {
    union conv_bits{
        double val;
        uint64_t bits;
    };
};


template <typename T>
struct Conv_info {};

template <>
struct Conv_info<float> {
    static const int sign_size =1;
    static const int exp_size = 8;
    static const int mant_size = 23;
    static const int total = sign_size + exp_size + mant_size;
};


template <>
struct Conv_info<double> {
    static const int sign_size =1;
    static const int exp_size = 11;
    static const int mant_size = 52;
    static const int total = sign_size + exp_size + mant_size;
};



template <typename conv_bits_type,typename conv_info_type>
uint32_t get_sign(conv_bits_type cb,conv_info_type ci) {
    uint32_t mask = 1<< ci.sign_size;
    uint32_t shift = ci.total - ci.sign_size;
    return (cb.bits >> shift) & mask  ; 
}

template <typename conv_bits_type,typename conv_info_type>
uint32_t get_exp(conv_bits_type cb,conv_info_type ci) {
    uint32_t mask = 1 << ci.exp_size;
    uint32_t shift = ci.total - ci.exp_size - ci.sign_size;
    return (cb.bits >> shift) &  mask ; 
}


template <typename conv_bits_type,typename conv_info_type>
uint32_t get_mant(conv_bits_type cb,conv_info_type ci) {
    uint32_t mask = 1 << ci.mant_size;
    return cb.bits & mask; 
}




int main () {
    vector<float> vct{-9.0,10.0};// seems to be veeery close to having this correctly represented as grouped binary representations
    int N = vct.size();//magic
    // minders would be used for helping to shift number insertion to next frame, and position correctly
    // TODO remove magic numbers
    vector<uint32_t> store(N);//QUESTION how should I deduce the type of the stored vector from our original's type?

    std::fill(store.begin(),store.end(),0);
    Conv_info<float> conv_info;
    // will use the conv info to substitute these magic number lines
    insert_minder<float> sign_inserter(0,1);//magic
    insert_minder<float> exp_inserter(N,8);//magic
    insert_minder<float> mant_inserter(8*N+N,23);//magic
    cbit_holder<float> cb_holder;
    for (float ele:  vct) {
        cb_holder.conv_bits.val = ele;
        uint32_t sign = get_sign(cb_holder.conv_bits,conv_info);
        sign_inserter.add_in(store,sign);
        uint32_t exp = get_exp(cb_holder.conv_bits,conv_info);
        exp_inserter.add_in(store,exp);
        uint32_t mant = get_mant(cb_holder.conv_bits,conv_info);
        mant_inserter.add_in(store,mant);
    }
    std::cout << "---" << std::endl;
    std::copy(store.begin(),store.end(),std::ostream_iterator<unsigned int>(std::cout, " "));
    std::cout << "---" << std::endl;
}


