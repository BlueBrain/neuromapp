/*
 * Neuromapp - memory.h, Copyright (c), 2015,
 * Tim ewart - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
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

/**
 * @file neuromapp/nest/synapse/memory.h
 * \brief allocator for the memory
 */


#ifndef MEMORY_H_
#define MEMORY_H_

#include <algorithm>
#ifdef _OPENMP
#include <omp.h>
#endif

namespace nest{

    struct cleaner{
        void operator()(void *ptr){
            free(ptr);
        }
    };


    /**
     * The poor man's allocator is a simple pool-based allocator, used to
     * allocate storage for connections in the limit of large machines.
     *
     * The allocator only supports allocation, but no freeing.  In the
     * limit of large machines this is sufficient, because we rarely need
     * to grow synapse lists, as the majority of neurons have at most one
     * target per machine.
     * The allocator manages the pool of free memory in chunks that form a
     * linked list.  A head pointer keeps track of the next position in a
     * chunk to be handed to the caller of allocate. Once head reaches the
     * end of the current chunk, a new chunk is allocated from the OS and
     * appends it to the linked list of chunks.
     */
    class PoorMansAllocator
    {
    private:
        /**
         * A chunk of memory, one element in the linked list of the memory
         * pool.
         */
        struct chunk
        {
            chunk( char* mem, chunk* next )
            : mem_( mem )
            , next_( next ){};
            char* mem_;
            chunk* next_;
        };

    public:
        PoorMansAllocator(): states(false){
        }

        ~PoorMansAllocator(){
        }

        /**
         * No constructors, as this would be counted as a 'use' of the
         * pool before declaring it thread-private by the compiler.
         * Therefore we have our own init() and destruct() functions.
         */
        void init( size_t chunk_size = 1048576 /** 1 mega Byte */){ 
            if (!states)
                save_ptr.reserve(256);

            capacity_ = 0;
            head_ = 0;
            chunks_ = 0;
            chunk_size_ = chunk_size;
            total_capacity_ = 0;
        }

        void destruct(){

            if(!states) {
                if (save_ptr.size() > 0)
                    std::for_each(save_ptr.begin(),save_ptr.end(),cleaner());
                save_ptr.clear();
            }
            else {
                for ( chunk* chunks = chunks_; chunks != 0; chunks = chunks->next_ )
                    free( chunks->mem_ );
            }
        }

        void* alloc( size_t obj_size ){
            char* ptr = head_;

            if(!states){
                ptr = (char*)malloc(obj_size);
                save_ptr.push_back((void*)ptr);
            }else{
                /** if the object allocated as larger size than a chu(n)ck
                 it will create a memory corruption */
                if(obj_size > chunk_size_)
                    throw std::bad_alloc();

                if ( obj_size > capacity_ ){
                    new_chunk();
                    total_capacity_ += chunk_size_;
                }
                ptr = head_;
                head_ += obj_size; // Advance pointer to next free location.
                // This works, because sizeof(head*) == 1
                capacity_ -= obj_size;
            }
            return ptr;
        }

        /** get function for the tests only*/
        size_t capacity() const{
            return capacity_;
        }

        size_t chunk_size() const{
            return chunk_size_;
        }

        size_t total_capacity() const{
            return total_capacity_;
        }

        /** states */
        bool states;
    private:
        /** I am not guilty od the NEST design */
        std::vector<void*> save_ptr;
        /**
         * Append a new chunk of memory to the list forming the memory
         * pool.
         */
        void new_chunk(){
            // We store the head pointer as char*, because sizeof(char) = 1, so
            // that we can add sizeof(object) to advance the pointer to the next
            // free location.
            head_ = reinterpret_cast< char* >( malloc( chunk_size_ ) );
            chunks_ = new chunk( head_, chunks_ );
            capacity_ = chunk_size_;
        }

        /**
         * The size of each chunk to be allocated. This size must be
         * chosen as a tradeoff between few allocations of chunks (large
         * chunk) and little memory overhead due to unused chunks (small
         * chunks).
         */
        size_t chunk_size_;

        /**
         * Points to the next free location in the current chunk. This
         * location will be handed to the caller in the next call of
         * allocate. The type is char*, so that pointer arithmetic allows
         * adding sizeof(object) directly to advance the pointer to the
         * next free location.
         */
        char* head_;
        
        /**
         * First element of the linked list of chunks.
         */
        chunk* chunks_;
        
        /**
         * Remaining capacity of the current chunk.
         */
        size_t capacity_;


        /**
         * Remaining capacity of the memory pool.
         */
        size_t total_capacity_;
    };

    //static std::vector<PoorMansAllocator> * poormansallocpool; = std::vector<PoorMansAllocator>(0);
    extern std::vector<PoorMansAllocator> poormansallocpool;

    class pool_env{
        const int num_threads_;
    public:
        pool_env(const int& num_threads=1, bool pool=false): num_threads_(num_threads)
        {
            poormansallocpool.resize(num_threads_);

            #pragma omp parallel for schedule(static, 1)
             for (int thrd=0; thrd<num_threads_; thrd++) {
                poormansallocpool[thrd].states = pool;
                poormansallocpool[thrd].init();
            }
        }
        ~pool_env(){
             #pragma omp parallel for schedule(static, 1)
             for (int thrd=0; thrd<num_threads_; thrd++) {
                poormansallocpool[thrd].destruct();
             }
        }
    };

    template < typename Tnew, typename Told, typename C >
    inline Tnew*
    suicide_and_resurrect( Told* connector, C connection )
    {
        #ifdef _OPENMP
        const int thrd = omp_get_thread_num();
        #else
        const int thrd = 0;
        #endif

        Tnew* p = NULL;
    //  #pragma omp critical // not thread safe!!
        {
        p = new ( poormansallocpool[thrd].alloc( sizeof( Tnew ) ) )
        Tnew(*connector, connection );
        connector->~Told(); // Needed otherwise destructor is not called of object
                            // memory and object handling is separated due to memory pool
        //now object is destructed but memory is still allocated
        //deallocation with current pool not possible
        }
        return p;
    }

    template < typename T, typename C >
    inline T*
    allocate( C c )
    {
        #ifdef _OPENMP
        const int thrd = omp_get_thread_num();
        #else
        const int thrd = 0;
        #endif

        T* p = NULL;
//agma omp critical // not thread safe!!
        {
        p = new ( poormansallocpool[thrd].alloc( sizeof( T ) ) ) T( c );
        }
      return p;
    }

    template < typename T, typename C >
    inline T*
    allocate()
    {
        #ifdef _OPENMP
        const int thrd = omp_get_thread_num();
        #else
        const int thrd = 0;
        #endif

        T* p = NULL;
#pragma omp critical // not thread safe!!
        {
        p = new ( poormansallocpool[thrd].alloc( sizeof( T ) ) ) T();
        }
      return p;
    }

}
#endif /* MEMORY_H_ */
