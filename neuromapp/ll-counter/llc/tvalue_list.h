#ifndef TVALUE_LIST_H
#define TVALUE_LIST_H

/** Simple utility class for representing and iterating over
 *  a list of values at compile-time.
 */

namespace llc {

template <typename T,T...>
struct tvalue_list {
    typedef T value_type;

    enum :bool { empty=true };
    enum :unsigned { length=0 };

    template <T p>
    using prepend = tvalue_list<T,p>;

    template <template <T> class F>
    struct for_each {
        template <typename... Args>
        static void run(Args&&... args) {}
    };
};

template <typename T,T h,T... t>
struct tvalue_list<T,h,t...> {
    typedef T value_type;

    enum :bool { empty=false };
    enum :unsigned { length=1+sizeof...(t) };

    constexpr static T head=h;
    typedef tvalue_list<T,t...> tail;

    template <T p>
    using prepend = tvalue_list<T,p,h,t...>;

    template <template <T> class F>
    struct for_each {
        template <typename... Args>
        static void run(Args&&... args) {
            F<h>::run(std::forward<Args>(args)...);
            tail::template for_each<F>::run(std::forward<Args>(args)...);
        }
    };
};


// sequence (inclusive) of values of (arithmetic) type T

namespace impl {
    template <typename T,T a,T b,int step,bool empty>
    struct tvalue_seq_impl {
        typedef tvalue_list<T> type;
    };

    template <typename T,T a,T b,int step>
    struct tvalue_seq_impl<T,a,b,step,false> {
        typedef typename tvalue_seq_impl<T,a+step,b,step,
            (step>0?(a+step>b):(a+step<b))>::type::template prepend<a> type;
    };
}

template <typename T,T a,T b,int step=1>
struct tvalue_seq {
    typedef typename impl::tvalue_seq_impl<T,a,b,step,(step>0?(a>b):(a<b))>::type type;
};

} // namespace llc

#endif // ndef TVALUE_LIST_H

