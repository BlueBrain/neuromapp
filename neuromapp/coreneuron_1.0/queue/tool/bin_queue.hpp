
#ifndef bin_queue_hpp_
#define bin_queue_hpp_

#include "coreneuron_1.0/queue/tool/bin_queue.hpp"

namespace tool {

/** the bin queue is a a kind of priority_queue using a ring concept, elements are sorted through
    bin from smallest to largest, into a bin there is NO specific order*. The determination
    of the bin is choosen by a kind of hash function (the hash give the bin "bucket").
    Into a bin with have a single link list using the "left" link of the bin_node class.

    Objectively this queue is designed only for our problem because we are hashing "time"
    using the inverse of dt. Consequently, genericity with template is useless

    I remove the array and use a std::vector to have a safe resize
 */
 
     //node for the bin queue
    template<class T>
    struct bin_node {
        typedef T value_type;
        explicit bin_node(value_type t = value_type()):t_(t),left_(0),cnt_(-1){};
        value_type t_;
        bin_node* left_;
        int cnt_;
    };

    template<class T>
    class bin_queue {
    public:
        typedef T value_type;
        typedef std::size_t size_type;
        typedef bin_node<value_type> node_type;

        inline explicit bin_queue(double dt = 0.025, value_type t0 = 0.):size_(0),qpt_(0),dt_(dt),tt_(t0)
                                                                             ,bins_(1024){}

        ~bin_queue();

        /** std::priority_queue API like */
        inline void push(value_type t){
            node_type* n = new node_type(t);
            enqueue(t,n); // t encapsulate in the bin_node but also needed for the "hash function"
            size_++;
        }

        inline void push(node_type* n){
            enqueue(n->t_,n); // t encapsulate in the bin_node but also needed for the "hash function"
            size_++;
        }

        inline void pop(){
            if(!empty()){
                node_type* q = first();
                remove(q);
                delete q;
                size_--;
            }
        }

        /* the top corresponds to the beginning of the queue as here we are mimic std::priority_queue with 
         the greated comparator */
        inline value_type top(){
            value_type r = value_type();
            if(!empty())
                r = first()->t_;
            return r;
        }

        inline size_type size(){
            return size_;
        }

        inline bool empty(){
            return !bool(size_); // is it true on Power?
        }
        
        inline node_type* find(node_type* n){
            remove(n);
            size_--; // WARNING remove the node but do not delete it
            return n;
        }

    private:
        /** original API */
        inline void enqueue(value_type tt, tool::bin_node<value_type>*);

        // for intenal only
        node_type* first();
        node_type* next(node_type*);
        void remove(node_type*);
    
        size_type size_;
        int qpt_; // unused here
        double dt_; // step times
        value_type tt_; // time at beginning of qpt_ interval
        std::vector<node_type*> bins_; // for correct resize
    };
}

#include "bin_queue.ipp"

#endif
