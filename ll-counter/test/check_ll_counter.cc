#include <iostream>

#include "ll_counter.h"
#include "ll_counter_generic_perf.h"

using namespace llc;

int main() {
    ll_counter<generic_perf> C(perf_event_sw(PERF_COUNT_SW_CPU_CLOCK));
    C.start();
    double dt=busy_wait(1e-3);
    C.stop();
    
    std::cout << "busy_wait() ns: " << dt*1e9 << "\n";
    std::cout << "C.count(): " << C.count() << "\n";

    ll_timer<generic_perf> T(perf_event_sw(PERF_COUNT_SW_CPU_CLOCK));
    T.estimate_dt(5e-3);

    std::cout << "estimated T.dt: " << T.dt << "\n";
    T.start();
    dt=busy_wait(1e-3);
    T.stop();

    std::cout << "busy_wait() ns: " << dt*1e9 << "\n";
    std::cout << "T.elapsed(): " << T.elapsed() << "\n";
}
