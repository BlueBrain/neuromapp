/*
 * Neuromapp - allocator.h, Copyright (c), 2015,
 * All rights reserved.
 *
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
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
#ifndef neuromap_ALLOCATOR
#define neuromap_ALLOCATOR

#include <cstdlib>
#include <stdlib.h> // POSIX, size_t are inside
#include <cstring>  // memset


#include "compression/exception.h"


namespace neuromapp {

    class cstandard {
        public:
            typedef std::size_t size_type;

            /**
             * allocate_policy 
             *
             *
             * @brief The allocate policy for cstandard blocks.
             *
             * @param size_type size
             *
             * @return void *
             */
            void * allocate_policy(size_type size) {
                if ((size & 0x8000000000000000)) // size is uint64_t so I check MSB if a negative number is the arg
                    throw cpu_memory_allocation("negative size, cpu cstandard policy", lengine_error::NEUROMAPP_BAD_ARG);
                else if (size == 0)
                    throw cpu_memory_allocation("null size, cpu cstandard policy", lengine_error::NEUROMAPP_BAD_ARG);

                void *ptr = std::malloc(size);
                std::memset(ptr, 0, size);
                /* log.insert(size, "cstandard"); */
                return ptr;
            }

            /**
             * deallocate_policy 
             *
             *
             * @brief The deallocate function used in the policy 
             *
             * @param void *ptr
             *
             * @return void
             */
            void deallocate_policy(void *ptr) { std::free(ptr); }

            /**
             * copy_policy 
             *
             *
             * @brief The copy function used in the policy 
             *
             * @param void *ptr_destination, void *ptr_source, std::size_t size
             *
             * @return void
             */
            void copy_policy(void *ptr_destination, void *ptr_source, std::size_t size) {
                std::memcpy(ptr_destination, ptr_source, size);
            }

            /**
             * compare_policy 
             *
             *
             * @brief The function used for comparing two different blocks of standard allocation.
             *
             * @param void * lhs,const void * rhs,size_t size
             *
             * @return bool
             */
            bool compare_policy(void * lhs,const void * rhs,size_t size) {
                int rc =std::memcmp(lhs,rhs,size);
                if (rc == 0) 
                    return true;// typical char* comparison rules
                else {
                    std::cerr << "block comparison failed: memcmp of blocks failed" << std::endl;
                    return false;
                }
            }

            constexpr static char policy_name[] = "cstandard";
    };

    class align {
        public:
            typedef std::size_t size_type;
            const static size_type boundary = 16; // always multiple of 32 byte

            /**
             * allocate_policy 
             *
             *
             * @brief The allocate function used in the policy
             *
             * @param size_type size
             *
             * @return void *
             */
            void * allocate_policy(size_type size) {
                if ((size & 0x8000000000000000)) // size is uint64_t so I check MSB if a negative number is the arg
                    throw cpu_memory_allocation("negative size, cpu align policy", lengine_error::NEUROMAPP_BAD_ARG);
                else if (size == 0)
                    throw cpu_memory_allocation("null size, cpu align policy", lengine_error::NEUROMAPP_BAD_ARG);

                // what is getting figured out here?
                void *ptr = NULL;
                int rc = posix_memalign(&ptr, boundary, size); // 16,32,64 etc ...

                if (rc != 0)
                    throw cpu_memory_allocation("posix allocation fails, cpu align policy", lengine_error::NEUROMAPP_UNKNOWN_ERROR);

                std::memset(ptr, 0, size);
                /* log.insert(size, "align"); */

                return ptr;
            }

            /**
             * deallocate_policy 
             *
             *
             * @brief The method for freeing the memory associated with the block.
             *
             * @param void *ptr
             *
             * @return void
             */
            void deallocate_policy(void *ptr) { std::free(ptr); }

            /**
             * resize_policy 
             *
             *
             * @brief Resizes the aligned memory. For each raw align, we may need to add some paddint at the end .
             *
             * @param size_type size, size_type sizeof_T
             *
             * @return inline static size_type
             */
            inline static size_type resize_policy(size_type size, size_type sizeof_T) {
                if (sizeof_T > boundary)
                    throw cpu_memory_allocation("wierd, sizeof(type) larger than boundary", lengine_error::NEUROMAPP_BAD_USAGE);
                size_type element_per_register = boundary / sizeof_T;
                return (size / element_per_register + 1) * element_per_register;
            }

            /**
             * copy_policy 
             *
             *
             * @brief  The copy function used in the policy for align blocks .
             *
             * @param void *ptr_destination, void *ptr_source, std::size_t size
             *
             * @return void
             */
            void copy_policy(void *ptr_destination, void *ptr_source, std::size_t size) {
                std::memcpy(ptr_destination, ptr_source, size);
            }


            /**
             * compare_policy 
             *
             *
             * @brief The align block compare policy.
             *
             * @param void * lhs,const void * rhs,size_t size
             *
             * @return bool
             */
            bool compare_policy(void * lhs,const void * rhs,size_t size) {
                cstandard temp_cstd_inst;
                return temp_cstd_inst.compare_policy(lhs,rhs,size);
            }

            constexpr static char policy_name[] = "align";

    };

} // namespace neuromapp

#endif
