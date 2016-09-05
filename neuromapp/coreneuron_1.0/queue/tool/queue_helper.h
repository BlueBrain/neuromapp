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

#ifndef tqueue_helper_h_
#define tqueue_helper_h_

namespace tool { // namespace si better than C style

    //node for the bin queue
    template<class T>
    struct bin_node {
        typedef T value_type;
        explicit bin_node(value_type t = value_type()):t_(t),left_(0),cnt_(-1){};
        value_type t_;
        bin_node* left_;
        int cnt_;
    };

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

    /** Forward declarations for the c++ interface, useless but give definition
    of this function */
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

    /* delete a specific node in the tree */
    template<class T>
    void spdelete(sptq_node<T>*, SPTREE<T>*);
    
} // end namespace

#endif
