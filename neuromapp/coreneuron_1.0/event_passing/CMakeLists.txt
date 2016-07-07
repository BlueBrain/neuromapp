include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})


#ENVIRONMENT LIBRARY
add_library (coreneuron10_environment environment/generator.cpp
                                      environment/presyn_maker.cpp)

install (TARGETS coreneuron10_environment DESTINATION lib)
install (FILES environment/generator.h
	       environment/event_generators.hpp
               environment/presyn_maker.h DESTINATION include)


#QUEUEING LIBRARY
add_library (coreneuron10_queueing queueing/thread.cpp
                                   queueing/queue.cpp)

install (TARGETS coreneuron10_queueing DESTINATION lib)
install (FILES queueing/pool.h
               queueing/pool.ipp
               queueing/thread.h
               queueing/queue.h DESTINATION include)
target_link_libraries (coreneuron10_queueing
                       coreneuron10_environment
                       coreneuron10_solver
                       coreneuron10_cstep
                       storage)

#SPIKE LIBRARY
install (FILES spike/algos.hpp
               spike/spike_interface.h DESTINATION include)

#APP
install (FILES drivers/drivers.h DESTINATION include)

add_library (coreneuron10_event drivers/main.cpp)

add_executable(event_exec drivers/event.cpp)
target_link_libraries (event_exec
                       coreneuron10_queueing
                       coreneuron10_environment
                       ${MPI_CXX_LIBRARIES}
                       ${MPI_C_LIBRARIES})

add_executable(dist_exec drivers/distributed_driver.cpp)
target_link_libraries (dist_exec
                       coreneuron10_queueing
                       coreneuron10_environment
                       ${MPI_CXX_LIBRARIES}
                       ${MPI_C_LIBRARIES})

install (TARGETS event_exec dist_exec DESTINATION bin)
