#include <boost/atomic.hpp>

#ifndef lockless_h
#define lockless_h

template<typename T>
class waitfree_queue {
public:
    struct node{
        T data;
        node* next;
    };

    void push(const T &data){
        node* n = new node;
        n->data = data;
        node* stale_head = head_.load(boost::memory_order_relaxed);
        do {
            n->next = stale_head;
	} while (!head_.compare_exchange_weak(stale_head, n, boost::memory_order_release));
    }
    waitfree_queue() : head_(0) {}

    // alternative interface if ordering is of no importance
    node* pop_all(void){
	return head_.exchange(0, boost::memory_order_consume);
    }

private:
   boost::atomic<node*> head_;

};

#endif
