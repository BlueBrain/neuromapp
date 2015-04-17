#include "llc/ll_counter.h"
#include "llc/ll_counter_generic_perf.h"

#include "gtest/gtest.h"

#include <iostream>

using namespace llc;

TEST(ll_timer,clock_perf_counter) {
    typedef ll_counter<generic_perf> counter_t;
    counter_t C(perf_event_sw(PERF_COUNT_SW_CPU_CLOCK));

    const double busy_dt=1e-2; // ten milliseconds
    double dt1,dt2;
    counter_t::value_type c1,c2;

    C.start();
    dt1=busy_wait(busy_dt);
    C.stop();
    c1=C.count();

    C.resume();
    dt2=busy_wait(busy_dt);
    C.stop();
    c2=C.count();

    // Expect busy_wait not to take more than a millisecond
    // over busy_dt.

    EXPECT_GT(dt1,busy_dt);
    EXPECT_GT(dt2,busy_dt);
    EXPECT_NEAR(dt1,busy_dt,1e-3);

    // Expect non-zero, monotonically increasing count values

    EXPECT_GT(c1,0);
    EXPECT_GT(c2,0);
    EXPECT_GT(c2,c1);

    // expect c1/c2 to be approximately dt1/(dt1+dt2)

    double expected_cratio=dt1/(dt1+dt2);
    double cratio=(double)c1/(double)c2;

    EXPECT_NEAR(cratio,expected_cratio,expected_cratio*0.05);
}

TEST(ll_timer,timer_estimation) {
    typedef ll_timer<generic_perf> timer_t;
    timer_t T(perf_event_sw(PERF_COUNT_SW_CPU_CLOCK));

    const double estimate_dt=5e-3; // five milliseconds
    const double busy_dt=1e-2;     // ten milliseconds
    double dt;

    T.estimate_dt(estimate_dt);
    T.start();
    dt=busy_wait(busy_dt);
    T.stop();

    // expect timer value to be close to reported dt
    EXPECT_NEAR(T.elapsed(),dt,dt*0.05);
}
