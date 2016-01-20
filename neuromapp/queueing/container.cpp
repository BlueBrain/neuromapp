/*
 * Neuromapp - container.cpp, Copyright (c), 2015,
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
 * @file neuromapp/queueing/container.cpp
 * \brief Contains Queue and Event class implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "queueing/container.h"

Queue::Queue() {
    least_ = 0;
}

Queue::~Queue() {
    while(pq_que.size()) {
        pq_que.pop();
    }
}

void Queue::insert(double tt, double d) {
    Event *i = new Event(d,tt);
    if(!least_){
        least_ = i;
    }
    else{
        if(tt < least_->t_){
            pq_que.push(make_QPair(least_));
            least_ = i;
	}
	else{
            pq_que.push(make_QPair(i));
        }
    }
}

Event *Queue::atomic_dq(double tt) {
    Event *q = 0;
    if (least_ && least_->t_ <= tt) {
        q = least_;
            while(pq_que.size() && pq_que.top().second->t_ < 0.)
            {
                pq_que.pop();
            }
            if(pq_que.size()) {
                least_ = pq_que.top().second;
                pq_que.pop();
            }else{
                least_ = NULL;
            }
    }
    return q;
}

