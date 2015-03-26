#include <chrono>
#include <iostream>

#include "ll_counter.h"
#include "linux_perf_count.h"

using namespace llc;

int main() {
    using namespace std;
    typedef chrono::duration<double> real_seconds;
    typedef chrono::duration<double,micro> real_us;

    perf_event_set P={perf_event_sw(PERF_COUNT_SW_CPU_CLOCK)};
    real_seconds req_dt(1e-3); // 1 millisecond

    ll_compiler_fence();
    chrono::time_point<chrono::high_resolution_clock> t0=chrono::high_resolution_clock::now();
    P.sample();
    uint64_t clk0=P[0];
    real_seconds rep_dt(busy_wait(req_dt.count()));

    P.sample();
    uint64_t clk1=P[0];
    chrono::time_point<chrono::high_resolution_clock> t1=chrono::high_resolution_clock::now();
    ll_compiler_fence();

    real_us req_dt_us=req_dt;
    real_us rep_dt_us=rep_dt;
    real_us measured=t1-t0;

    cout << "req_dt   (µs): " << req_dt_us.count() << "\n";
    cout << "rep_dt   (µs): " << rep_dt_us.count() << "\n";
    cout << "measured (µs): " << measured.count() << "\n";
    cout << "clk delta: " << (clk1-clk0) << "\n";
}
    

