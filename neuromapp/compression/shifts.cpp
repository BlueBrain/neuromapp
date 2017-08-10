#include <iostream>
#include <iterator>
#include <cmath>
#include <algorithm>
#include <vector>
#include <cstdint>
#include <cstdlib>
using namespace std;


template <typename T>
struct insert_minder { 
    int v_i;    
    int pos;
    int rel_start;// relative starting position
    int shift_type;
    public:
    insert_minder(int rel_start_arg,int shift_type_arg) :rel_start {rel_start_arg},shift_type{shift_type_arg} {
        v_i = floor(rel_start/Conv_info<T>::total);// tells us which element in storage vector we start adding into
        pos = (Conv_info<T>::total-1)-rel_start%Conv_info<T>::total;// what position in that vector element to add 
    }

    void add_in(vector<typename Conv_info<T>::bytetype> & vct, typename Conv_info<T>::bytetype val ) {
        for (int i = 1; i <= shift_type;i++) {
            typename Conv_info<T>::bytetype frame_val = (val >> (shift_type -i)) & 1 ;
            vct[v_i] += frame_val << pos ;
            pos--;
            if (pos < 0) {// move to the start of the next number
                v_i++;
                pos = Conv_info<T>::total-1;
            }
        }
    }

    void take_out(vector<T> & vct, typename Conv_info<T>::bytetype val) {
        for (int i = 1; i <= shift_type;i++) {
            typename Conv_info<T>::bytetype frame_val = (val >> i) & 1 ;
            vct[v_i] += frame_val << pos ;
            pos++;
            if (pos > 31) {// move to the start of the next number
                v_i--;
                pos = 0;
            }
        }

    }

};

template <typename T>
struct cbit_holder {};

template <>
struct cbit_holder<float> {
    union {
        float val;
        uint32_t bits;
    }conv_bits;
};


template <>
struct cbit_holder<double> {
    union {
        double val;
        uint64_t bits;
    }conv_bits;
};


template <typename T>
struct Conv_info {};

template <>
struct Conv_info<float> {
    static const int sign_size =1;
    static const int exp_size = 8;
    static const int mant_size = 23;
    static const int total = sign_size + exp_size + mant_size;
    typedef uint32_t bytetype;
};


template <>
struct Conv_info<double> {
    static const int sign_size =1;
    static const int exp_size = 11;
    static const int mant_size = 52;
    static const int total = sign_size + exp_size + mant_size;
    typedef uint64_t bytetype;
};



template <template T>
typename Conv_info<T>::bytetype get_sign(T val) {
    cbit_holder<T> cb;
    cb.ele = val;
    typename Conv_info<T>::bytetype mask = 1<< Conv_info<T>::sign_size;
    typename Conv_info<T>::bytetype shift = Conv_info<T>::total - Conv_info<T>::sign_size;
    return (cb.bits >> shift) & mask  ; 
}

template <template T>
typename Conv_info<T>::bytetype get_exp(T val) {
    cbit_holder<T> cb;
    cb.ele = val;
    typename Conv_info<T>::bytetype mask = 1 << Conv_info<T>::exp_size;
    typename Conv_info<T>::bytetype shift = Conv_info<T>::total - Conv_info<T>::exp_size - Conv_info<T>::sign_size;
    return (cb.bits >> shift) &  mask ; 
}


template <typename conv_bits_type,typename conv_info_type>
typename Conv_info<T>::bytetype get_mant(conv_bits_type cb,conv_info_type ci) {
    typename Conv_info<T>::bytetype mask = 1 << ci.mant_size;
    return cb.bits & mask; 
}




int main () {
    typedef float vct_type;
    vector<vct_type> vct{-9.0,10.0};// seems to be veeery close to having this correctly represented as grouped binary representations
    int N = vct.size();
    // minders would be used for helping to shift number insertion to next frame, and position correctly
    // TODO remove magic numbers
    vector<typename Conv_info<vect_type>::bytetype> store(N);//QUESTION how should I deduce the type of the stored vector from our original's type?

    std::fill(store.begin(),store.end(),0);
    Conv_info<vct_type> conv_info;
    // will use the conv info to substitute these magic number lines
    insert_minder<vct_type> sign_inserter(0,Conv_info<vect_type>::sign_size);
    insert_minder<vct_type> exp_inserter(N,Conv_info<vect_type>::exp_size);
    insert_minder<vct_type> mant_inserter(Conv_info<vect_type>::exp_size*N+N,Conv_info<vect_type>::mant_size);
    cbit_holder<vct_type> cb_holder;
    for (vct_type ele:  vct) {
         typename Conv_info<vect_type>::bytetype sign = get_sign(ele);
         sign_inserter.add_in(store,sign);
         typename Conv_info<vect_type>::bytetype exp = get_exp(ele);
         exp_inserter.add_in(store,exp);
         typename Conv_info<vect_type>::bytetype mant = get_mant(ele);
         mant_inserter.add_in(store,mant);
    }
    std::cout << "---" << std::endl;
    std::copy(store.begin(),store.end(),std::ostream_iterator<unsigned int>(std::cout, " "));
    std::cout << "---" << std::endl;
    vector<vct_type> vct2(N);
    for (typename Conv_info<vect_type>::bytetype ele:  store) {
    }
    std::cout << "---" << std::endl;
    std::copy(vct2.begin(),vct2.end(),std::ostream_iterator<unsigned int>(std::cout, " "));
    std::cout << "---" << std::endl;
}


