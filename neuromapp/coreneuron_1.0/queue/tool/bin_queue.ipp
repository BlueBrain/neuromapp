
#ifndef bin_queue_ipp_
#define bin_queue_ipp_

#include "coreneuron_1.0/queue/tool/algorithm.h" //for the sptq::bin_node object

namespace tool{

    template<class T>
    bin_queue<T>::~bin_queue() {
        node_type* q, *q2;
        for (q = first(); q; q = q2) {
            q2 = next(q);
            remove(q); /// Potentially dereferences freed pointer this->sptree_
            delete q;
        }
    }

    template<class T>
    void bin_queue<T>::enqueue(T td, node_type* q) {

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
    typename bin_queue<T>::node_type* bin_queue<T>::first() {
        for (int i = qpt_; i < bins_.size(); ++i) {
            if (bins_[i]) {
                return bins_[i];
            }
        }
        return 0;
    }

    template<class T>
    typename bin_queue<T>::node_type* bin_queue<T>::next(node_type* q) {
        if (q->left_) { return q->left_; }
        for (int i = q->cnt_ + 1; i < bins_.size(); ++i) {
            if (bins_[i]) {
                return bins_[i];
            }
        }
        return 0;
    }

    template<class T>
    void bin_queue<T>::remove(node_type* q) {
        node_type* q1, *q2;
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
