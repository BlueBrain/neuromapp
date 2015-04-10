add_library(llc STATIC
    linux_perf_count.cc
    ll_counter.cc
    pin_thread.cc)

set(llc_public_headers
    linux_perf_count.h  ll_common.h
    ll_counter.h        ll_counter_generic_perf.h
    ll_counter_power8.h ll_counter_x86.h
    ll_harness.h        pin_thread.h
    tvalue_list.h       unroll.h)

include_directories(..)

install(TARGETS llc DESTINATION lib)
install(FILES ${llc_public_headers} DESTINATION include/llc)

