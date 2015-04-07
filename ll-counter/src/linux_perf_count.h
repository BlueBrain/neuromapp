#ifndef LINUX_PERF_COUNT_H_
#define LINUX_PERF_COUNT_H_

#include <cstdint>
#include <memory>
#include <unistd.h>
#include <linux/perf_event.h>

namespace llc {

struct perf_event {
    uint32_t type;
    uint64_t config,config1,config2;
};

// add more as required ...

static inline perf_event perf_event_hw(uint64_t c1,uint64_t c2=0,uint64_t c3=0) {
    return perf_event{PERF_TYPE_HARDWARE,c1,c2,c3};
}

static inline perf_event perf_event_sw(uint64_t c1,uint64_t c2=0,uint64_t c3=0) {
    return perf_event{PERF_TYPE_SOFTWARE,c1,c2,c3};
}

static inline perf_event perf_event_raw(uint64_t c1,uint64_t c2=0,uint64_t c3=0) {
    return perf_event{PERF_TYPE_RAW,c1,c2,c3};
}

/** Enable and support reading through kernel interface
 *  hardware cycle and op counts
 */

struct perf_event_set {
    size_t nev;
    size_t rdbuf_sz;
    std::unique_ptr<uint64_t[]> rdbuf;

    struct ev_data {
        perf_event ev_spec;
        int fd;
        void *map;
    };
    std::unique_ptr<ev_data[]> ev;

    size_t map_sz;

    perf_event_set(std::initializer_list<perf_event> L):
        nev(L.size()),
        rdbuf_sz(sizeof(uint64_t)*(nev+1)),
        rdbuf(new uint64_t[nev+1]),
        ev(new ev_data[nev])
    {
        auto item=L.begin();
        for (size_t i=0;i<nev;++i) ev[i].ev_spec=*item++;
        register_events();
    }

    explicit perf_event_set(perf_event p): perf_event_set({p}) {}

    perf_event_set(perf_event_set &&) = default;

    perf_event_set(const perf_event_set &) = delete;
    perf_event_set &operator=(const perf_event_set &) = delete;

    /** Sample registered counters.
     *
     * @return Returns true on success.
     */

    bool sample() {
        int r=read(ev[0].fd,rdbuf.get(),rdbuf_sz);
        return (size_t)r==rdbuf_sz;
    }

    /** Enable counters in set */

    void enable();

    /** Disable counters in set */

    void disable();

    /** Retrieve last sampled value of counter */

    uint64_t operator[](int i) const {
        return rdbuf[i+1];
    }

    /** Return pointer to perf mmap page */

    perf_event_mmap_page *map(int i) {
        return (perf_event_mmap_page *)ev[i].map;
    }

    ~perf_event_set() { cleanup(); }

private:
    void register_events();
    void cleanup();

};

} // namespace llc

#endif // ndef  LINUX_PERF_COUNT_H_
