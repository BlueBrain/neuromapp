
#ifndef bin_queue_ipp_
#define bin_queue_ipp_

#include "coreneuron_1.0/queue/tool/queue_helper.h" //for the sptq::node object

namespace tool{

    template<class T>
    bin_queue<T>::~bin_queue() {
        tool::node<T>* q, *q2;
        for (q = first(); q; q = q2) {
            q2 = next(q);
            remove(q); /// Potentially dereferences freed pointer this->sptree_
        }
    }

    template<class T>
    void bin_queue<T>::enqueue(T td, tool::node<T>* q) {

        int rev_dt = 1/dt_;
        int idt = (int)((td - tt_)*rev_dt + 1.e-10);
        if(idt >= bins_.size())
            bins_.resize(idt<<1); //double the size
        assert(idt >= 0);
        if(idt < qpt_)
            qpt_ = idt; // keep track of the first bin
//        idt += qpt_;
//        if (idt >= bins_.size()) { idt -= bins_.size(); } // is it relevant now ?
//        assert (idt < bins_.size());
        q->cnt_ = idt; // only for iteration
        q->left_ = bins_[idt];
        bins_[idt] = q;
    }

    template<class T>
    tool::node<T>* bin_queue<T>::first() {
        for (int i = qpt_; i < bins_.size(); ++i) {
            if (bins_[i]) {
                return bins_[i];
            }
        }
        return 0;
    }

    template<class T>
    tool::node<T>* bin_queue<T>::next(tool::node<T>* q) {
        if (q->left_) { return q->left_; }
        for (int i = q->cnt_ + 1; i < bins_.size(); ++i) {
            if (bins_[i]) {
                return bins_[i];
            }
        }
        return 0;
    }

    template<class T>
    void bin_queue<T>::remove(tool::node<T>* q) {
        tool::node<T>* q1, *q2;
        q1 = bins_[q->cnt_];
        if (q1 == q) {
            bins_[q->cnt_] = q->left_;
            return;
        }
        for (q2 = q1->left_; q2; q1 = q2, q2 = q2->left_) {
            if (q2 == q) {
                q1->left_ = q->left_;
                return;
            }   
        }   
    }
}

#endif
