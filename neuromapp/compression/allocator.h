#ifndef neuromap_ALLOCATOR
#define neuromap_ALLOCATOR

#include <cstdlib>
#include <stdlib.h> // POSIX, size_t are inside
#include <cstring>  // memset

#include "exception.h"

namespace neuromapp {

class cstandard {
  public:
    typedef std::size_t size_type;

    //note that the lengine entries used to be neuromap - w/o extra p
    //todo remove the mention of lengine anywhere
    /** The allocate function used in the policy */
    void *allocate_policy(size_type size) {
        if ((size & 0x8000000000000000)) // size is uint64_t so I check MSB if a negative number is the arg
            throw cpu_memory_allocation("negative size, cpu cstandard policy", lengine_error::NEUROMAPP_BAD_ARG);
        else if (size == 0)
            throw cpu_memory_allocation("null size, cpu cstandard policy", lengine_error::NEUROMAPP_BAD_ARG);

        void *ptr = std::malloc(size);
        std::memset(ptr, 0, size);
        /* log.insert(size, "cstandard"); */
        return ptr;
    }

    /** The deallocate function used in the policy */
    void deallocate_policy(void *ptr) { std::free(ptr); }

    /** The copy function used in the policy */
    void copy_policy(void *ptr_destination, void *ptr_source, std::size_t size) {
        std::memcpy(ptr_destination, ptr_source, size);
    }

    constexpr static char policy_name[] = "cstandard";
};

class align {
  public:
    typedef std::size_t size_type;
    const static size_type boundary = 32; // always multiple of 32 byte

    /** The allocate function used in the policy */
    void *allocate_policy(size_type size) {
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

    /** The deallocate function used in the policy */
    void deallocate_policy(void *ptr) { std::free(ptr); }

    /** to have each raw align, we may need to add some paddint at the end */
    inline static size_type resize_policy(size_type size, size_type sizeof_T) {
        if (sizeof_T > boundary)
            throw cpu_memory_allocation("wierd, sizeof(type) larger than boundary", lengine_error::NEUROMAPP_BAD_USAGE);
        size_type element_per_register = boundary / sizeof_T;
        return (size / element_per_register + 1) * element_per_register;
    }

    /** The copy function used in the policy */
    void copy_policy(void *ptr_destination, void *ptr_source, std::size_t size) {
        std::memcpy(ptr_destination, ptr_source, size);
    }

    constexpr static char policy_name[] = "align";
};

} // namespace neuromapp

#endif
