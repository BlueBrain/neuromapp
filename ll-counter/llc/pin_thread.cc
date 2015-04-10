#include <vector>
#include <cerrno>
#include <stdexcept>
#include <system_error>

#include <sched.h>

#include "pin_thread.h"

// platform specific:

#undef PIN_THREAD_GLIBC

#if __GLIBC__==2 && __GLIBC_MINOR__>-7
#define PIN_THREAD_GLIBC
#else
#error "No pin_thread.cc implementation for this platform"
#endif

#ifdef PIN_THREAD_GLIBC

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <sched.h>

namespace llc {

int thread_cpu() {
    int cpu=sched_getcpu();
    if (cpu<0) throw std::system_error(errno,std::system_category());
    return cpu;
}

void pin_thread() { return pin_thread(thread_cpu()); }

void pin_thread(int cpu) {
    if (cpu<0 || cpu>=CPU_SETSIZE) throw std::range_error("cpu number out of range");

    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(cpu,&set);

    if (sched_setaffinity(0,sizeof(set),&set)<0)
        throw std::system_error(errno,std::system_category());
}

affinity_set_t thread_affinity() {
    affinity_set_t affinity;

    cpu_set_t set;
    if (sched_getaffinity(0,sizeof(set),&set)<0)
        throw std::system_error(errno,std::system_category());

    affinity.reserve(CPU_COUNT(&set));

    for (int i=0;i<CPU_SETSIZE;++i) if (CPU_ISSET(i,&set)) affinity.push_back(i);
    return affinity;
}

} // namespace llc
    
#endif
