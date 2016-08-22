
#ifndef bin_queue_hpp_
#define bin_queue_hpp_

namespace tool {

/** the bin queue is a a kind of priority_queue using a ring concept, elements are sorted through
    bin from smallest to largest, into a bin there is NO specific order*. The determination
    of the bin is choosen by a kind of hash function (the hash give the bin "bucket").
    Into a bin with have a single link list using the "left" link of the node class.

    Objectively this queue is designed only for our problem because we are hashing "time"
    using the inverse of dt. Consequently, genericity with template is useless

    I remove the array and use a std::vector to have a safe resize
 */

    template<class T>
    class bin_queue {
    public:
        typedef T value_type;
        typedef std::size_t size_type;

        inline explicit bin_queue(double dt = 0.025, value_type t0 = 0.):size_(0),qpt_(0),dt_(dt),tt_(t0)
                                                                             ,bins_(1024,NULL){}

        ~bin_queue();

        /** std::priority_queue API like */
        inline void push(value_type t){
            tool::node<value_type>* n = new tool::node<value_type>(t);
            enqueue(t,n); // t encapsulate in the node but also needed for the "hash function"
            size_++;
        }

        inline void pop(){
            if(!empty()){
                tool::node<value_type>* q = first();
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
                r = first()->key();
            return r;
        }

        inline size_type size(){
            return size_;
        }

        inline bool empty(){
            return !bool(size_); // is it true on Power?
        }

        /** original API */
        inline void enqueue(value_type tt, tool::node<value_type>*);

        // for intenal only
        tool::node<value_type>* first();
        tool::node<value_type>* next(tool::node<value_type>*);
        void remove(tool::node<value_type>*);
    private:
        size_type size_;
        int qpt_; // unused here
        double dt_; // step times
        value_type tt_; // time at beginning of qpt_ interval
        std::vector<tool::node<value_type>* > bins_; // for correct resize
    };
}


#include "bin_queue.ipp"
    
#endif
