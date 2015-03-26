#include <cstring>
#include <cassert>
#include <cstdint>
#include <cerrno>
#include <stdexcept>
#include <system_error>

#include <linux/perf_event.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include "linux_perf_count.h"

namespace llc {

void perf_event_set::register_events() {
    map_sz=(size_t)sysconf(_SC_PAGESIZE);
    for (int i=0;i<nev;++i) {
        ev[i].fd=-1;
        ev[i].map=0;
    }

    perf_event_attr pe_common;

    memset(&pe_common,0,sizeof(pe_common));
    pe_common.size=sizeof(pe_common);
    pe_common.exclude_kernel=1;
    pe_common.exclude_hv=1;
    pe_common.exclude_idle=1;
    pe_common.read_format=PERF_FORMAT_GROUP;

    for (int i=0;i<nev;++i) {
        perf_event_attr pe=pe_common;
        pe.type=ev[i].ev_spec.type;
        pe.config=ev[i].ev_spec.config;
        pe.config1=ev[i].ev_spec.config1;
        pe.config2=ev[i].ev_spec.config2;

        int fd_grp=(i==0)?-1:ev[0].fd;
        ev[i].fd=syscall(__NR_perf_event_open,&pe,0,-1,fd_grp,0);
        if (ev[i].fd<0) goto cleanup_and_throw_syserror;

        ev[i].map=mmap(0,map_sz,PROT_READ,MAP_SHARED,ev[i].fd,0);
    }

    return;

cleanup_and_throw_syserror:
    int err=errno;
    cleanup();
    throw std::system_error(err,std::system_category());
}

void perf_event_set::enable() {
    if (!ev || !nev) return;
    ioctl(ev[0].fd,PERF_EVENT_IOC_ENABLE,0);
}

void perf_event_set::disable() {
    if (!ev || !nev) return;
    ioctl(ev[0].fd,PERF_EVENT_IOC_DISABLE,0);
}


void perf_event_set::cleanup() {
    // check first that we haven't been moved or already cleaned up.
    if (!ev) return;

    size_t i=nev;
    while (i>0) {
        --i;
        if (ev[i].map) munmap(ev[i].map,map_sz);
        if (ev[i].fd>=0) close(ev[i].fd);
    }

    rdbuf.release();
    ev.release();
}

} // namespace llc
