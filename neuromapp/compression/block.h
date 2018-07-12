/*
 * Neuromapp - block.h, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
 *  -original block.h
 *  -policy design strategy for development
 * Devin Bayly - University of Arizona
 * baylyd@email.arizona.edu,
 *  -sorting tools
 *  -block IO
 *  -compressor policy
 *  -benchmarking (STREAM,kernel measure, etc)
 *
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */
#ifndef NEUROMAPP_BLOCK
#define NEUROMAPP_BLOCK

#include <string>
#include <memory> // POSIX, size_t is inside
#include <functional>
#include <sstream>
#include <iterator>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>
#include <type_traits>
#include <cctype>
#include <cassert>

#include "compression/type_definition.h"
#include "compression/allocator.h" 
#include "compression/compressor.h"
#include "compression/exception.h"

namespace neuromapp {

    // other allocator nothing special col is not modify
    template <class Allocator>
        /**
         * resize_helper 
         *
         *
         * @brief Base resize function for align specification later on.
         *
         * @param std::size_t n, std::size_t sizeof_T
         *
         * @return inline std::size_t
         */
        inline std::size_t resize_helper(std::size_t n, std::size_t sizeof_T) {
            return n;
        }

    // specific to align allocator
    template <>
        /**
         * resize_helper<align> 
         *
         *
         * @brief Specialized resize_helper for align blocks. Makes use of the resize policy provided in allocator.h.
         *
         * @param std::size_t n, std::size_t sizeof_T
         *
         * @return inline std::size_t
         */
        inline std::size_t resize_helper<align>(std::size_t n, std::size_t sizeof_T) {
            return align::resize_policy(n, sizeof_T);
        }

    template <class T= typename memory_policy_type::value_type, class allocator = typename memory_policy_type::block_allocator_type,class compressor = typename memory_policy_type::block_compressor_type>
        class block : public allocator, compressor {
            using allocator::allocate_policy;
            using allocator::deallocate_policy;
            using allocator::copy_policy;
            using allocator::compare_policy;
            //expose compressor functions
            using compressor::compress_policy;
            using compressor::uncompress_policy;

            public:
            typedef std::size_t size_type;
            typedef allocator allocator_type;
            // seems like T was only typedef'd for use in other typedefs, interesting
            typedef T value_type;
            typedef value_type *pointer;
            typedef pointer iterator;
            typedef iterator * col_iterator;
            typedef const value_type *const_pointer;
            typedef value_type &reference;
            typedef const value_type &const_reference;

            // m = 1 one instance only !
            // constructor given dimensions
            /**
             * block constructor 
             *
             *
             * @brief This is the standard block constructor with defaults of 1 row and 1 column. Note the columns come first in the argument order.
             *
             * @param size_type n = 1, size_type m = 1) : rows_(m
             *
             * @return 
             */
            block(size_type n = 1, size_type m = 1) : rows_(m) {
                dim0_ = n;                                      // dim0 not necessary = num_cols due to the resize
                cols_ = resize_helper<allocator>(n, sizeof(T)); // some policy will resize the col, needs for 2D !
                current_size = sizeof(T) * cols_ * rows_;
                data_ = (pointer)allocate_policy(current_size);
            }

            //constructor given rval to another block
            /**
             * block constructor 
             *
             *
             * @brief This is the copy constructor. We provide other block, and constructor generates new block with same dimensions, and data, from it.
             *
             * @param block &&other) : rows_(other.rows_), cols_(other.cols_), dim0_(other.dim0_), data_(other.data_
             *
             * @return 
             */
            block(block &&other) : rows_(other.rows_), cols_(other.cols_), dim0_(other.dim0_), data_(other.data_) {
                // std::move is not needed on basic type
                other.rows_ = 0;
                other.cols_ = 0;
                other.dim0_ = 0;
                current_size = 0;
                other.data_ = nullptr;
            }

            /**
             * block constructor 
             *
             *
             * @brief Similar to the (&& other) copy constructor, bu uses const reference instead.
             *
             * @param const block &other
             *
             * @return 
             */
            block(const block &other) {
                // std::move is not needed on basic type
                rows_ = other.rows_;
                cols_ = other.cols_;
                dim0_ = other.dim0_;
                current_size = sizeof(T) * cols_ * rows_;
                data_ = (pointer)allocate_policy(current_size);
                copy_policy(data_, other.data_, current_size);
            }

            /**
             * operator= 
             *
             *
             * @brief This is the standard move assignment operator. Leaves the rhs reference empty to avoid unexpected behavior from assigned block.
             *
             * @param block &rhs
             *
             * @return block & 
             */
            block & operator=(block &rhs) {
                rows_ = rhs.rows_;
                cols_ = rhs.cols_;
                dim0_ = rhs.dim0_;
                data_ = rhs.data_;
                current_size = rhs.current_size;
                rhs.rows_ = 0;
                rhs.cols_ = 0;
                rhs.dim0_ = 0;
                rhs.data_ = nullptr;
                return *this;
            }


            //copy = operator
            /**
             * operator= 
             *
             *
             * @brief Copy operator, uses right hand reference to make assignment.
             *
             * @param block &&rhs
             *
             * @return block &
             */
            block &operator=(block &&rhs) {
                rows_ = rhs.rows_;
                cols_ = rhs.cols_;
                dim0_ = rhs.dim0_;
                data_ = rhs.data_;
                current_size = rhs.current_size;
                rhs.rows_ = 0;
                rhs.cols_ = 0;
                rhs.dim0_ = 0;
                rhs.data_ = nullptr;

                // avoid destruction here



                return *this;
            }

            ~block() {
                if (data_ != nullptr) // due to move constructor
                    deallocate_policy(data_);
            }


            /**
             * resize 
             *
             *
             * @brief This function shrinks or expands the space allowed for holding data within the block.
             *
             * @param size_type n = 1, size_type m = 1
             *
             * @return void
             */
            void resize(size_type n = 1, size_type m = 1) {
                // essentially just getting which is smaller the resize or the current
                if (n != dim0() || m != dim1()) {
                    const auto copy_cols = n < dim0() ? n : dim0();
                    const auto copy_rows = m < dim1() ? m : dim1();

                    // new block will be destroy at the end of the scopr
                    // and data are echange due to the std::move and consequent std::swap
                    block new_block(n, m); // function probably lives in allocator

                    for (size_type r(0); r < copy_rows; ++r)
                        copy_policy(&new_block[r], &(*this)[r], sizeof(T) * copy_cols);

                    *this = std::move(new_block);
                }
            }






            /**
             * begin 
             *
             *
             * @brief This function allows for STL like algorithms to iterate element by element over the block starting with the return from begin().
             *
             * @param 
             *
             * @return iterator
             */
            iterator begin() { return data_;}

            /**
             * end 
             *
             *
             * @brief This function returns the sentinel end for comparing an iterator over the block to. 
             *
             * @param 
             *
             * @return iterator
             */
            iterator end() { return data_ + dim0_ * rows_;}


            class iter : public std::iterator<std::bidirectional_iterator_tag,value_type,size_type> {
                block<value_type, allocator_type> * blk;
                size_type row_limit = blk->num_rows();
                size_type col_limit = blk->dim0();
                size_type col_ind,row_mult;
                value_type comp_val;
                public:

                iter (block<value_type,allocator_type>*  blk_in, size_type col,size_type comp_row,size_type row) 
                    : blk {blk_in} ,col_ind {col}, row_mult{row} {
                        comp_val = (*blk)(col_ind,comp_row);
                    }

                /**
                 * get_value 
                 *
                 *
                 * @brief This function gets the value the iterator dereferences to.
                 *
                 * @param 
                 *
                 * @return const value_type&
                 */
                const value_type& get_value() const {
                    return comp_val;
                }

                /**
                 * get_col 
                 *
                 *
                 * @brief Gives back the column number that the iterator is currently on
                 *
                 * @param 
                 *
                 * @return const size_type &
                 */
                const size_type & get_col () const {
                    return col_ind;
                }

                /**
                 * set_col 
                 *
                 *
                 * @brief positions the iterator on a particular column within the block.
                 *
                 * @param const size_type & col
                 *
                 * @return void
                 */
                void set_col (const size_type & col) {
                    col_ind = col;
                }

                /**
                 * operator ++  
                 *
                 *
                 * @brief The increment operator moves the iterator down the row.
                 *
                 * @param int
                 *
                 * @return iter 
                 */
                iter operator ++ (int) {
                    iter temp(blk,col_ind,row_mult);
                    row_mult++;
                    return temp;
                }

                /**
                 * operator --  
                 *
                 *
                 * @brief  Decrement function for iterator, moves back one in the row. Returns a reference to the iterator. prefix decrement
                 *
                 * @param 
                 *
                 * @return iter& 
                 */
                iter& operator -- () {
                    row_mult--;
                    return *this;
                }

                /**
                 * operator--   
                 *
                 *
                 * @brief  Postfix iterator decrement operator. returns iterator, not reference.
                 *
                 * @param int
                 *
                 * @return iter 
                 */
                iter operator--  (int) {
                    iter temp(blk,col_ind,row_mult);
                    row_mult--;
                    return temp;
                }

                /**
                 * operator ++  
                 *
                 *
                 * @brief  Prefix increment operator for the iterator. Also checks whether limit has been reached for incrementing.
                 *
                 * @param 
                 *
                 * @return iter& 
                 */
                iter& operator ++ () {
                    if (row_mult > row_limit) throw std::out_of_range("row mult too hi");

                    row_mult++;
                    return *this;
                }

                /**
                 * operator*  
                 *
                 *
                 * @brief  Dereference operator for the iterator. Gives a reference back to the value in the block at the point where the
                 * iterator is.
                 *
                 * @param 
                 *
                 * @return value_type& 
                 */
                value_type& operator* () {
                    // make use of the () operator from block
                    return (*blk)(col_ind,row_mult);
                }

                /**
                 * operator*  
                 *
                 *
                 * @brief Dereference operator for the iterator, returns const reference to the value that the iterator is referencing within the block.
                 *
                 * @param 
                 *
                 * @return const value_type& 
                 */
                const value_type& operator* () const {
                    // make use of the () operator from block
                    return (*blk)(col_ind,row_mult);
                }


                /**
                 * operator ==  
                 *
                 *
                 * @brief   The equals comparison operator. If the positions within the row are identical for the two iterators, returns true.
                 *
                 * @param const iter &rhs
                 *
                 * @return bool 
                 */
                bool operator == (const iter &rhs) {
                    return row_mult == rhs.row_mult;
                }
                /**
                 * operator !=  
                 *
                 *
                 * @brief The not-equals comparison operator. Returns whether the value pointed to is different between the two iterators.
                 *
                 * @param const iter &rhs
                 *
                 * @return bool 
                 */
                bool operator != (const iter &rhs) {
                    return row_mult != rhs.row_mult;
                }

                /**
                 * operator =  
                 *
                 *
                 * @brief Assignment operator for the iterator. Copies all salient values from the rhs iterator, and assigns them to the lhs iterator.
                 *
                 * @param const iter &rhs
                 *
                 * @return iter& 
                 */
                iter& operator = (const iter &rhs) {
                    row_mult = rhs.row_mult;
                    col_ind = rhs.col_ind;
                    comp_val = rhs.comp_val;
                    return *this;
                }

                // last things needed by a bidirectional iterator
                /**
                 * operator +=  
                 *
                 *
                 * @brief Assign-increment operator, allows for skipping positions within the block referenced by iterator.
                 *
                 * @param const iter & rhs
                 *
                 * @return iter & 
                 */
                iter & operator += (const iter & rhs) {
                    if(row_mult > row_limit) throw std::out_of_range("row mult too hi");
                    row_mult += rhs.row_mult;
                    return *this;
                }

                template<typename num_t>
                    /**
                     * operator +=  
                     *
                     *
                     * @brief Assign-increment operator, uses numeric type for its increment guide.
                     *
                     * @param const num_t & rhs
                     *
                     * @return iter & 
                     */
                    iter & operator += (const num_t & rhs) {
                        if(row_mult > row_limit) throw std::out_of_range("row mult too hi");
                        row_mult += (size_type) rhs;
                        return *this;
                    }

                template<typename num_t>
                    /**
                     * operator -=  
                     *
                     *
                     * @brief Decrement-assign operator. This moves the iterator backwards down a row in steps that can be larger
                     * than one at a time.
                     *
                     * @param const num_t & rhs
                     *
                     * @return iter & 
                     */
                    iter & operator -= (const num_t & rhs) {
                        row_mult -= (size_type) rhs;
                        return *this;
                    }

                /**
                 * operator -  
                 *
                 *
                 * @brief Subtraction operator, useful for defining ranges necessary for the random access iterator type
                 *
                 * @param const iter &rhs
                 *
                 * @return size_type 
                 */
                size_type operator - (const iter &rhs) {
                    return row_mult - rhs.row_mult;
                }

                /**
                 * operator + 
                 *
                 *
                 * @brief Addition operator, useful for defining ranges necessary for the random access iterator type
                 *
                 * @param  const iter & rhs
                 *
                 * @return size_type 
                 */
                size_type operator +( const iter & rhs) {
                    return row_mult - rhs.row_mult;
                }

                /**
                 * operator []  
                 *
                 *
                 * @brief Indexing operator. Specify block row address to get value of from iterator.
                 *
                 * @param const size_type & row_ind
                 *
                 * @return value_type 
                 */
                value_type operator [] (const size_type & row_ind) {
                    // make use of the existing + operator and *
                    return (*blk)(col_ind,row_ind);
                }

                /**
                 * operator ->  
                 *
                 *
                 * @brief Dagger operator. This returns a pointer to a position within the block.
                 *
                 * @param 
                 *
                 * @return pointer 
                 */
                pointer operator -> () const {
                    return &(*blk)[row_mult];
                }

                template<typename num_t>
                    /**
                     * operator +  
                     *
                     *
                     * @brief commutative + operators for the iterator, used in defining Ranges for random access iterator.
                     *
                     * @param const num_t & lhs
                     *
                     * @return iter 
                     */
                    iter operator + (const num_t & lhs) const {
                        return iter(blk,col_ind,row_mult + lhs);
                    }

                template<typename num_t>
                    /**
                     * operator -  
                     *
                     *
                     * @brief  commutative - operators for the iterator, used in defining Ranges for random access iterator.
                     *
                     * @param  const num_t & lhs
                     *
                     * @return iter 
                     */
                    iter operator - ( const num_t & lhs) const {
                        return iter(blk,col_ind,row_mult - lhs);
                    }

                //total ordering operators

                /**
                 * operator >  
                 *
                 *
                 * @brief Relational operator. Determines whether the current iterator is at a position further in the row than the rhs iterator.
                 *
                 * @param const iter & rhs
                 *
                 * @return bool 
                 */
                bool operator > (const iter & rhs) const{
                    return row_mult > rhs.row_mult;
                }
                /**
                 * operator <  
                 *
                 *
                 * @brief Relational operator. Determines whether the current iterator is not as far in the row as the rhs iterator.
                 *
                 * @param const iter & rhs
                 *
                 * @return bool 
                 */
                bool operator < (const iter & rhs) const{
                    return row_mult < rhs.row_mult;
                }
                /**
                 * operator <=  
                 *
                 *
                 * @brief Relational operator. Determines if current iterator is at a position closer to the start or equal to the rhs iterator.
                 *
                 * @param const iter & rhs
                 *
                 * @return bool 
                 */
                bool operator <= (const iter & rhs) const{
                    return row_mult <= rhs.row_mult;
                }
                /**
                 * operator >=  
                 *
                 *
                 * @brief Relational operator. Determines if current iterator is at a position further from the start or equal to the rhs iterator.
                 *
                 * @param const iter & rhs
                 *
                 * @return bool 
                 */
                bool operator >= (const iter & rhs) const{
                    return row_mult >= rhs.row_mult;
                }
            };

            /**
            * print_row 
            *
            *
            * @brief This function prints one row from the block using the arguments provided.
            *
            * @param size_type row,std::string && mesg,size_type cols
            *
            * @return void
            */
            void print_row(size_type row,std::string && mesg,size_type cols); 






            //difference between memory_allocated and size is that allocated relies on construction size, where size depends on compression
            /**
             * memory_allocated 
             *
             *
             * @brief Returns the memory used to hold teh data within the block currently
             *
             * @param 
             *
             * @return size_type
             */
            size_type memory_allocated() const { return sizeof(T) * cols_ * rows_; }

            /**
            * is_compressed 
            *
            *
            * @brief
            *
            * @param 
            *
            * @return bool
            */
            bool is_compressed() {return compression_state;}

            /**
            * get_current_size 
            *
            *
            * @brief
            *
            * @param 
            *
            * @return size_type
            */
            size_type get_current_size() {return current_size;}

            /**
             * data 
             *
             *
             * @brief This function provides a const pointer to the block of memory used for block data storage. 
             *
             * @param 
             *
             * @return const_pointer
             */
            const_pointer data() const { return data_; };

            /**
             * data 
             *
             *
             * @brief Returns a pointer to the memory used for block data storage.
             *
             * @param 
             *
             * @return pointer
             */
            pointer data() { return data_; };

            /**
             * dim0 
             *
             *
             * @brief Returns the number of columns in the block, buffered for align or not.
             *
             * @param 
             *
             * @return size_type
             */
            size_type dim0() const { return dim0_; }
            /**
             * dim1 
             *
             *
             * @brief Return the number of rows it the block.
             *
             * @param 
             *
             * @return size_type
             */
            size_type dim1() const { return rows_; }// supposed to be dim1?
            // alias
            /**
             * num_cols 
             *
             *
             * @brief Returns the number of cols_, not taking into account the buffering done for align allocator
             * blocks.
             *
             * @param 
             *
             * @return size_type
             */
            size_type num_cols() const { return cols_; }
            /**
             * num_rows 
             *
             *
             * @brief Returns the number of rows used in the block. Essentially the same as calling dim1, but easier to remember.
             *
             * @param 
             *
             * @return size_type
             */
            size_type num_rows() const { return rows_; }

            /**
             * operator[](size_type i) { return (*this) 
             *
             *
             * @brief The index operator for the block. return a reference to the data stored at that address in the block. Only for first element in column.
             *
             * @param 0, i
             *
             * @return reference 
             */
            reference operator[](size_type i) { return (*this)(0, i); }

            /**
             * operator[](size_type i) const { return (*this) 
             *
             *
             * @brief The index operator for block. Const reference return from address given by argument address. Only for first element in column.
             *
             * @param 0, i
             *
             * @return const_reference 
             */
            const_reference operator[](size_type i) const { return (*this)(0, i); }

            /**
             * operator() 
             *
             *
             * @brief call operator. Returns a reference to the element stored at the part of the block.
             *
             * @param size_type i, size_type j = 0
             *
             * @return reference 
             */
            reference operator()(size_type i, size_type j = 0) {
                // determines if i j are legal
                assert(i <= cols_);
                assert(j <= rows_);
                return data_[i + j * cols_];
            }

            /**
             * operator() 
             *
             *
             * @brief call operator. Returns a const_reference to the element stored at the part of the block.
             *
             * @param size_type i, size_type j = 0
             *
             * @return const_reference 
             */
            const_reference operator()(size_type i, size_type j = 0) const {
                assert(i <= cols_);
                assert(j <= rows_);
                return data_[i + j * cols_];
            }

            //when given the std::cout for the standard operator we add to its stream
            /**
             * print 
             *
             *
             * @brief Print out the entire block element by element to ostream os.
             *
             * @param std::ostream & os
             *
             * @return std::ostream &
             */
            std::ostream & print(std::ostream & os) const {
                for (int i = 0; i < dim1(); ++i) { // raw first
                    for (int j = 0; j < dim0(); ++j) {
                        //precision defines accuracy for testing comparison
                        os <<std::setprecision(15) << (*this)(j, i);
                        os << (j != dim0()-1 ? " " : "");// prevent trailing whitespace in output
                    }
                    os << (i != dim1() -1? "\n": "") ;
                }
                return os;
            }

            /**
             * operator ==  
             *
             *
             * @brief Uses compare policy specified in teh allocator.h to determine block equality between two blocks.
             *
             * @param const block & other
             *
             * @return bool 
             */
            bool operator == (const block & other) {
                //check size matches first
                return compare_policy(this->data(),other.data(),current_size);
            }

            /**
             * operator !=  
             *
             *
             * @brief Uses binary negation on the return from the compare policy given current block, and other.
             *
             * @param const block & other
             *
             * @return bool 
             */
            bool operator != (const block & other) {
                //should just be the opposite of the existing compare_policy
                return ! compare_policy(this->data(),other.data(),current_size);
            }




            // this is the tool for adding entries to our block
            void read(std::istream & file_in); 

            // block access to compression functions included via policy
            // make into data ref and size as arguments
            //
            /**
            * compress 
            *
            *
            * @brief
            *
            * @param 
            *
            * @return void
            */
            void compress(); 
            /**
            * uncompress 
            *
            *
            * @brief
            *
            * @param 
            *
            * @return void
            */
            void uncompress() ;

            void fill_block(value_type val); 
            private:
            size_type rows_;
            size_type cols_;
            size_type dim0_;
            pointer data_;
            //compression members
            size_type current_size;
            bool compression_state = false;
        };
    //follow Tim's pattern with the outbound ostream above
    template <class T, class A>
            /**
            * operator >>  
            *
            *
            * @brief
            *
            * @param std::istream & in, block<T,A> &b 
            *
            * @return std::istream & 
            */
        std::istream & operator >> (std::istream & in, block<T,A> &b ) {
            b.read(in);// create contents of block based on data in inputstream
            return in;
        }


            template <class T, class A>
                /**
                 * operator<< 
                 *
                 *
                 * @brief This operator prints the block element by element.
                 *
                 * @param std::ostream &out, block<T, A> &b
                 *
                 * @return std::ostream &
                 */
                std::ostream &operator<<(std::ostream &out, block<T, A> &b) {
                    b.print(out);
                    return out;
                }

} // namespace neuromapp

//last but not least attach the .hpp file
#include "compression/block.hpp"
#endif
