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

//the node is too fat for the bin queue right and left useless
template<class T>
struct node {
    typedef T value_type;

    explicit node(value_type t = value_type()):t_(t),left_(0),right_(0),parent_(0),cnt_(-1){};
    // remove the macro "key" now key() is function but I keep the MH notation
    inline value_type key() { return t_;} const
	value_type t_;
	node* left_;
	node* right_;
	node* parent_;
    int cnt_;
};

template<class T>
struct SPTREE{
    node<T>	* root;		/* root node */
    int	enqcmps;	/* compares in spenq */
};

/** Forward declarations for the c++ interface */
/* init tree */
template<class T>
void spinit(SPTREE<T>*);

/* insert item into the tree */
template<class T, class Compare>
node<T>* spenq(node<T>*, SPTREE<T>*);

/* return and remove lowest item in subtree */
template<class T>
node<T>* spdeq(node<T>**);

/* return first node in tree */
template<class T>
node<T>* sphead(SPTREE<T>*);

} // end namespace

#endif
