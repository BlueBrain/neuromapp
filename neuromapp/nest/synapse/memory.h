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

namespace nest{

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
        PoorMansAllocator(){
            init();
        }


        ~PoorMansAllocator(){
            destruct();
        }

        /**
         * No constructors, as this would be counted as a 'use' of the
         * pool before declaring it thread-private by the compiler.
         * Therefore we have our own init() and destruct() functions.
         */
        void init( size_t chunk_size = 1048576 ){
            capacity_ = 0;
            head_ = 0;
            chunks_ = 0;
            chunk_size_ = chunk_size;
        }

        void destruct(){
            for ( chunk* chunks = chunks_; chunks != 0; chunks = chunks->next_ )
                free( chunks->mem_ );
        }

        void* alloc( size_t obj_size ){
            if ( obj_size > capacity_ )
                new_chunk();
            char* ptr = head_;
            head_ += obj_size; // Advance pointer to next free location.
            // This works, because sizeof(head*) == 1
            capacity_ -= obj_size;
            return ptr;
        }

    private:
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
    };

}
#endif /* MEMORY_H_ */
