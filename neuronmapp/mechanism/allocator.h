template<typename T>
class StandardAllocPolicy {
public : 
    //    typedefs
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

public : 
    //    convert an StandardAllocPolicy<T> to StandardAllocPolicy<U>
    template<typename U>
    struct rebind {
        typedef StandardAllocPolicy<U> other;
    };

public : 
    inline explicit StandardAllocPolicy() {}
    inline ~StandardAllocPolicy() {}
    inline explicit StandardAllocPolicy(StandardAllocPolicy const&) {}
    template <typename U>
    inline explicit StandardAllocPolicy(StandardAllocPolicy<U> const&) {}
    
    //    memory allocation
    inline pointer allocate(size_type cnt, 
      typename std::allocator<void>::const_pointer = 0) { 
      void* p;
      int k = posix_memalign((void **)&p, 16, cnt*sizeof(T));    
      if (k != 0) printf("Allocation of array a failed, return code is %d\n",k);
      char *m = static_cast<char*>(p);
      #pragma omp parallel for schedule(static)
      for(size_t i=0; i < cnt*sizeof(T) ; i++)
          m[i]=0; 
      if (!p)
            throw std::bad_alloc();
      return reinterpret_cast<pointer>(p);
    }

    inline void deallocate(pointer p, size_type){
        free(p);
     }

    //    size
    inline size_type max_size() const { 
        return std::numeric_limits<size_type>::max(); 
    }
};    //    end of class StandardAllocPolicy

// determines if memory from another
// allocator can be deallocated from this one
template<typename T, typename T2>
inline bool operator==(StandardAllocPolicy<T> const&, 
                        StandardAllocPolicy<T2> const&) { 
    return true;
}
template<typename T, typename OtherAllocator>
inline bool operator==(StandardAllocPolicy<T> const&, 
                                     OtherAllocator const&) { 
    return false; 
}
