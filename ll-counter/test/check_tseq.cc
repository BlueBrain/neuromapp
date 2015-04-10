#include <utility>
#include <iostream>
#include "tvalue_list.h"

using namespace llc;

typedef tvalue_seq<int,2,30,5>::type nums;

void emit() {
    std::cout << "\n";
}

template <typename A1>
void emit(A1&& a1) {
    std::cout << std::forward<A1>(a1) << "\n";
}

template <typename A1,typename A2,typename... Args>
void emit(A1&& a1,A2&& a2,Args&&... rest) {
    std::cout << std::forward<A1>(a1) << ",";
    emit(std::forward<A2>(a2),std::forward<Args>(rest)...);
}

template <int i>
struct foo {
    template <typename... Args>
    static void run(Args&&... args) {
        std::cout << i << ": ";
        emit(std::forward<Args>(args)...);
    }
};


int main() {
    nums::for_each<foo>::run("hello","world",3.2);
}


