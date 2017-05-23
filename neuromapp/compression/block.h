#ifndef NEUROMAPP_BLOCK
#define NEUROMAPP_BLOCK

#include <memory> // POSIX, size_t is inside
#include <iostream>
#include <type_traits>
#include <cassert>

#include "allocator.h" // that's right, this is another way to tie up files in different locations.
#include "exception.h"
#include "type_definition.h"

//tim uses comments to denote the end of meaningful brackets
namespace neuromapp {

    // other allocator nothing special col is not modify
    template <class Allocator>
        inline std::size_t resize_helper(std::size_t n, std::size_t sizeof_T) {
            return n;
        }

    // specific to align allocator
    template <>
        inline std::size_t resize_helper<align>(std::size_t n, std::size_t sizeof_T) {
            return align::resize_policy(n, sizeof_T);
        }

    //beginning of actual block class material
    template <class T, class allocator = typename memory_policy_type::block_type>
        class block : public allocator {
            using allocator::allocate_policy;
            using allocator::deallocate_policy;
            using allocator::copy_policy;

            public:
            typedef std::size_t size_type;
            typedef allocator allocator_type;
            // seems like T was only typedef'd for use in other typedefs, interesting
            typedef T value_type;
            typedef value_type *pointer;
            typedef pointer iterator;
            typedef const value_type *const_pointer;
            typedef value_type &reference;
            typedef const value_type &const_reference;

            // m = 1 one instance only !
            // constructor given dimensions
            block(size_type n = 1, size_type m = 1) : rows_(m) {
                dim0_ = n;                                      // dim0 not necessary = num_cols due to the resize
                cols_ = resize_helper<allocator>(n, sizeof(T)); // some policy will resize the col, needs for 2D !
                data_ = (pointer)allocate_policy(sizeof(T) * cols_ * rows_);
            }
            //constructor given rval to another block
            block(block &&other) : rows_(other.rows_), cols_(other.cols_), dim0_(other.dim0_), data_(other.data_) {
                // std::move is not needed on basic type
                other.rows_ = 0;
                other.cols_ = 0;
                other.dim0_ = 0;
                other.data_ = nullptr;
            }

            // todo ask tim does this work on blocks with data in them?
            block(const block &other) {
                // std::move is not needed on basic type
                rows_ = other.rows_;
                cols_ = other.cols_;
                dim0_ = other.dim0_;
                size_type size = sizeof(T) * cols_ * rows_;
                data_ = (pointer)allocate_policy(size);
                copy_policy(data_, other.data_, size);
            }


            //copy = operator
            block &operator=(block &&rhs) {
                rows_ = rhs.rows_;
                cols_ = rhs.cols_;
                dim0_ = rhs.dim0_;

                // avoid destruction here
                std::swap(data_, rhs.data_);

                return *this;
            }

            ~block() {
                if (data_ != nullptr) // due to move constructor
                    deallocate_policy(data_);
            }

            //this is the method that we call to read in as much data as we have space for in the block

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

            // stl compatibility
            iterator begin() { return data_; }
            iterator end() { return data_ + dim0_ * rows_; }

            size_type memory_allocated() const { return sizeof(T) * cols_ * rows_; }

            const_pointer data() const { return data_; };

            pointer data() { return data_; };

            size_type dim0() const { return dim0_; }
            size_type dim1() const { return rows_; }
            // alias
            size_type num_cols() const { return cols_; }
            size_type num_rows() const { return rows_; }

            reference operator[](size_type i) { return (*this)(0, i); }// what function is getting called here?

            const_reference operator[](size_type i) const { return (*this)(0, i); }

            reference operator()(size_type i, size_type j = 0) {
                // determines if i j are legal
                assert(i <= cols_);
                assert(j <= rows_);
                return data_[i + j * cols_];
            }

            const_reference operator()(size_type i, size_type j = 0) const {
                assert(i <= cols_);
                assert(j <= rows_);
                return data_[i + j * cols_];
            }

            //did I write this print thing?
            //since std::cout is being used, we don't need the parameter,
            void print() const {
                for (int i = 0; i < dim1(); ++i) { // raw first
                    for (int j = 0; j < dim0(); ++j) {
                        std::cout << (*this)(j, i) << " ";
                    }
                    std::cout << " \n";
                }
            }
            // this is the tool for adding entries to our block
            void enter_data(const std::string & fname)
            {
                //could use either the iterator, or the indexing, dunno which is better
                //note that this doesn't break if the file doesn't exist



                //check if the file exists
                struct stat file_check;
                if (stat(fname.c_str(),&file_check) !=0) {
                    throw std::runtime_error( std::string ("non-existing file ") + fname);
                }
                std::ifstream in_file(fname);
                std::cout << fname << std::endl;
                auto block_it = this->begin();
                std::string str_temp {""};
                while(in_file >> str_temp && block_it != this->end()){
                    int int_temp {stoi(str_temp)};
                    *block_it = int_temp;
                    block_it++;
                }
                // close the file
                in_file.close();

            }
            private:
            size_type rows_;
            size_type cols_;
            size_type dim0_;
            pointer data_;
        };

    template <class T, class A>
        std::ostream &operator<<(std::ostream &out, block<T, A> &b) {
            b.print(out);
            return out;
        }

} // namespace neuromapp


#endif
