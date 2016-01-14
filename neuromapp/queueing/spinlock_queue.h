#include <pthread.h>

#ifndef spinlock_h
#define spinlock_h

template<typename T>
class spinlock_queue {
public:
    struct node{
        T data;
        node* next;
    };

    spinlock_queue() : head_(NULL) {pthread_spin_init(&lock_,0);}

    ~spinlock_queue(){
	pthread_spin_destroy(&lock_);
	while(head_ != NULL){
	    node* temp = head_;
	    head_ = head_ -> next;
	    delete temp;
	}
    }

    void push(const T &data){
        node* n = new node;
        n->data = data;
        pthread_spin_lock(&lock_);
        n->next = head_;
	head_ = n;
	pthread_spin_unlock(&lock_);
    }

    node* pop_all(void){
        pthread_spin_lock(&lock_);
	node* original_head = head_;
	head_ = NULL;
        pthread_spin_unlock(&lock_);
	return original_head;
    }

private:
   pthread_spinlock_t lock_;
   node* head_;

};

#endif
