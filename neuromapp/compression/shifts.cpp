#include <iostream>
#include <iomanip>
#include <iterator>
#include <cmath>
#include <algorithm>
#include <vector>
#include <cstdint>
#include <cstdlib>
using namespace std;

template <typename T>
struct Conv_info {};

template <>
struct Conv_info<float> {
    static const int sign_size = 1;
    static const int exp_size = 8;
    static const int mant_size = 23;
    static const int total = sign_size + exp_size + mant_size;
    typedef uint32_t bytetype;
};


template <>
struct Conv_info<double> {
    static const int sign_size = 1;
    static const int exp_size = 11;
    static const int mant_size = 52;
    static const int total = sign_size + exp_size + mant_size;
    typedef uint64_t bytetype;
};



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

    void reset() {
        v_i = floor(rel_start/Conv_info<T>::total);// tells us which element in storage vector we start adding into
        pos = (Conv_info<T>::total-1)-rel_start%Conv_info<T>::total;// what position in that vector element to add 
    }


    void take_out(vector<typename Conv_info<T>::bytetype> & storage,typename Conv_info<T>::bytetype & outcome_num) {

        for (int i = 1; i <= shift_type;i++) {
            outcome_num = outcome_num << 1;
            typename Conv_info<T>::bytetype frame_val = (storage[v_i] >> pos) & 1 ;
            std::cout << frame_val;
            outcome_num += frame_val ;
            pos--;
            if (pos < 0) {// move to the start of the next number
                v_i++;
                pos = Conv_info<T>::total-1;
            }
        }
        std::cout << "" << std::endl;
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
typename Conv_info<T>::bytetype get_sign(T val) {
    cbit_holder<T> cb;
    cb.conv_bits.val = val;
    typename Conv_info<T>::bytetype mask = 1;
    typename Conv_info<T>::bytetype shift = Conv_info<T>::total - Conv_info<T>::sign_size;
    return (cb.conv_bits.bits >> shift) & mask  ; 
}

template <typename T>
typename Conv_info<T>::bytetype get_exp(T val) {
    cbit_holder<T> cb;
    cb.conv_bits.val = val;
    typename Conv_info<T>::bytetype mask = (1 << Conv_info<T>::exp_size)-1;
    typename Conv_info<T>::bytetype shift = Conv_info<T>::total - Conv_info<T>::exp_size - Conv_info<T>::sign_size;
    return (cb.conv_bits.bits >> shift) &  mask ; 
}


template <typename T>
typename Conv_info<T>::bytetype get_mant(T val) {
    cbit_holder<T> cb;
    cb.conv_bits.val=val;
    typename Conv_info<T>::bytetype mask = (1 << Conv_info<T>::mant_size)-1;
    return cb.conv_bits.bits & mask; 
}

template <typename T>
T get_dec(typename Conv_info<T>::bytetype bits){
    cbit_holder<T> cb;
    cb.conv_bits.bits=bits;
    return cb.conv_bits.val; 
}

int main () {
    typedef float vect_type;
    vector<vect_type> vct{-9.0,10.0};// seems to be veeery close to having this correctly represented as grouped binary representations
    int N = vct.size();
    // minders would be used for helping to shift number insertion to next frame, and position correctly
    // TODO remove magic numbers
    vector<typename Conv_info<vect_type>::bytetype> store(N);//QUESTION how should I deduce the type of the stored vector from our original's type?

    std::fill(store.begin(),store.end(),0);
    // will use the conv info to substitute these magic number lines
    insert_minder<vect_type> sign_inserter(0,Conv_info<vect_type>::sign_size);
    insert_minder<vect_type> exp_inserter(N,Conv_info<vect_type>::exp_size);
    insert_minder<vect_type> mant_inserter(Conv_info<vect_type>::exp_size*N+N,Conv_info<vect_type>::mant_size);
    typename Conv_info<vect_type>::bytetype sign; 
    typename Conv_info<vect_type>::bytetype exp; 
    typename Conv_info<vect_type>::bytetype mant;
    for (vect_type ele:  vct) {
         sign = get_sign(ele);
         sign_inserter.add_in(store,sign);
         exp = get_exp(ele);
         exp_inserter.add_in(store,exp);
         mant = get_mant(ele);
         mant_inserter.add_in(store,mant);
    }
    std::copy(store.begin(),store.end(),std::ostream_iterator<unsigned int>(std::cout, " "));
    std::cout << "" << std::endl;

    vector<vect_type> vct2(N);
    sign_inserter.reset();
    exp_inserter.reset();
    mant_inserter.reset();
    for (int i = 0;i< N ; i++) {
        typename Conv_info<vect_type>::bytetype aggregate = 0;
        sign_inserter.take_out(store,aggregate);
        std::cout << "agg" << aggregate << std::endl;
        exp_inserter.take_out(store,aggregate);
        std::cout << "agg" << aggregate << std::endl;
        mant_inserter.take_out(store,aggregate);
        std::cout << "agg" << aggregate << std::endl;
        vct2[i] = get_dec<vect_type>(aggregate); 
        std::cout << std::setprecision(5) << vct2[i] << std::endl;
    }
}


