/*
 * Neuromapp - queueing.h, Copyright (c), 2015,
 * Kai Langen - Swiss Federal Institute of technology in Lausanne,
 * kai.langen@epfl.ch,
 * All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/queueing/spinlock_queue.h
 * \brief Implementation of the queu without mutex
 */

#include <pthread.h>

#ifdef __APPLE__
#include "queueing/spinlock_apple.h"
#endif

#ifndef MAPP_SPINLOCK_QUEUE_
#define MAPP_SPINLOCK_QUEUE_

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
