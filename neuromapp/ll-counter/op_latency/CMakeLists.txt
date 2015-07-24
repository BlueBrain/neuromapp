add_executable(op_latency
    cxx_demangle.cc primitive_ops.cc run_kernels.cc)

include_directories(..)
target_link_libraries(op_latency llc rt)

install(TARGETS op_latency DESTINATION bin)

