#include_directories(${PROJECT_BINARY_DIR})
#include_directories(${PROJECT_SOURCE_DIR})


#ENVIRONMENT LIBRARY
add_library (coreneuron10_environment environment/generator.cpp
                                      environment/presyn_maker.cpp)

install (TARGETS coreneuron10_environment DESTINATION lib)
install (FILES environment/environment.h
               environment/generator.h
               environment/presyn_maker.h DESTINATION include)


#QUEUEING LIBRARY
add_library (coreneuron10_queueing queueing/pool.cpp
                                   queueing/thread.cpp
                                   queueing/queue.cpp)

install (TARGETS coreneuron10_queueing DESTINATION lib)
install (FILES queueing/pool.h
               queueing/thread.h
               queueing/queue.h DESTINATION include)
target_link_libraries (coreneuron10_queueing
                       coreneuron10_environment
                       coreneuron10_solver
                       coreneuron10_cstep
                       storage)

#SPIKE LIBRARY
install (FILES spike/algos.hpp
               spike/spike.h
               spike/spike_interface.h DESTINATION include)

#APP
add_executable(coreneuron10_event_app   drivers/main2.cpp)
target_link_libraries (coreneuron10_event_app
                       coreneuron10_queueing
                       coreneuron10_environment
                       ${MPI_CXX_LIBRARIES}
                       ${MPI_C_LIBRARIES})

install (TARGETS coreneuron10_event_app DESTINATION bin)


#Add spike to the mini-app library
#add_library (spike main.cpp)

#install (TARGETS spike DESTINATION lib)
#install (FILES spike.h DESTINATION include)

#Add mpispike to the mini-app library
#add_library (spike_exchange environment.cpp)

#install (TARGETS spike_exchange DESTINATION lib)
#install (FILES environment.h algos.hpp DESTINATION include)
#target_link_libraries (spike_exchange coreneuron10_queueing)

#Add an MPI executable
#add_executable(spike_exec mpi_spike_exec.cpp)
#target_link_libraries (spike_exec spike_exchange ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES})

#install (TARGETS spike_exec DESTINATION bin)
