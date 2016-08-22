


#if defined(__x86_64__)
    static __inline__ unsigned long long rdtsc(void){
        unsigned long long int upper, lower;
        __asm__ __volatile__ ("rdtsc" : "=a"(lower), "=d"(upper));
        return ( (unsigned long long)lower)|( ((unsigned long long)upper)<<32   );
    }
#elif defined(__powerpc__)
    static __inline__ unsigned long long rdtsc(void){
        unsigned long long int result=0;
        unsigned long int upper, lower,tmp;
        __asm__ volatile(
                     "0:                  \n"
                     "\tmftbu   %0           \n"
                     "\tmftb    %1           \n"
                     "\tmftbu   %2           \n"
                     "\tcmpw    %2,%0        \n"
                     "\tbne     0b         \n"
                     : "=r"(upper),"=r"(lower),"=r"(tmp)
                     );
        result = upper;
        result = result<<32;
        result = result|lower;
        return(result);
    }
#endif