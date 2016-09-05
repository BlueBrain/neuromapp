/*
Copyright (c) 2016, Blue Brain Project
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef tqueue_hpp_
#define tqueue_hpp_

#include <stdio.h>
#include <assert.h>
#include <cstring>
#include <ostream>
#include <functional>

#include "coreneuron_1.0/queue/tool/algorithm.h"

namespace tool {
/** The queue: TQeue from Michael starts here, not compliant with std for the container,
but ok for the type support and the comparator, by default std::less and not std::greated
as it was in the original version, already a bit of clean up specially for the push ... */

//node for the splay tree
template<class T>
struct sptq_node {
    typedef T value_type;
    explicit sptq_node(value_type t = value_type()):t_(t),left_(0),right_(0),parent_(0){};
    value_type t_;
    sptq_node* left_;
    sptq_node* right_;
    sptq_node* parent_;
};

template<class T>
struct SPTREE{
    sptq_node<T>	* root;		/* root node */
    int	enqcmps;	/* compares in spenq */
};

/** Forward declarations for the c++ interface */
/* init tree */
template<class T>
void spinit(SPTREE<T>*);

/* insert item into the tree */
template<class T, class Compare>
sptq_node<T>* spenq(sptq_node<T>*, SPTREE<T>*);

/* return and remove lowest item in subtree */
template<class T>
sptq_node<T>* spdeq(sptq_node<T>**);

/* return first node in tree */
template<class T>
sptq_node<T>* sphead(SPTREE<T>*);

/*return a looking node */
template<class T>
sptq_node<T>* spdelete(sptq_node<T>*,SPTREE<T>*);

template<class T = double, class Compare = std::less<T> >
class sptq_queue {
public:
    typedef tool::SPTREE<T> container;
    typedef std::size_t size_type;
    typedef T value_type;
    typedef sptq_node<T> node_type;

    inline sptq_queue():s(0) {
        tool::spinit(&q);
    }

    inline ~sptq_queue(){
        node_type *n;
        while((n = tool::spdeq(&(&q)->root)) != NULL)
          delete n;
    }

    inline void push(value_type value){
        node_type *n = new node_type(value);
        tool::spenq<T,Compare>(n, &q); // the Comparator is use only here
        s++;
    }

    inline void push(node_type& n) const{
        tool::spenq<T,Compare>(n, &q);
    }

    inline void pop(){
        if(!empty()){
            node_type *n = tool::spdeq(&(&q)->root);
            delete n; // pop remove definitively the element else memory leak
            s--;
        }
    }

    inline value_type top(){
        value_type tmp = value_type();
        if(!empty())
            tmp = tool::sphead<T>(&q)->t_;
        return tmp;
    }

    inline size_type size(){
        return s;
    }

    inline bool empty(){
        return !bool(s); // is it true on Power?
    }

    inline node_type* find(node_type& n){
        return spdelete(&n, &q);
    }

    void print(std::ostream &os) {
        while(!empty()){
            os << top() << std::endl;
            pop();
        }
    }

private:
    size_type s;
    container q;
};

// carefull the << delete the queue only for debugging 
template<class T, class Compare>
std::ostream& operator<< (std::ostream& os, sptq_queue<T,Compare>& q ){
    q.print(os);
    return os;
}

} // end namespace
#include "sptq_queue.ipp"

#endif
