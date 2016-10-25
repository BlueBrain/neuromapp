include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})

#NEST event_passing library
add_library (nest_event_passing eventdelivermanager.cpp
                          mpi_manager.cpp
                          simulationmanager.cpp)

install (TARGETS nest_event_passing DESTINATION lib)
install (FILES eventdelivermanager.h
               mpi_manager.h
               simulationmanager.h DESTINATION include)
               
target_link_libraries (nest_event_passing
                       coreneuron10_environment)