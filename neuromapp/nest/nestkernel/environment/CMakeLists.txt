include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})

#NEST environment library
add_library (nest_environment connectionmanager.cpp
                          connector_base.cpp
                          event.cpp
                          node.cpp
                          scheduler.cpp)

install (TARGETS nest_environment DESTINATION lib)
install (FILES connectionmanager.h
               connector_base.h
               event.h
               node.h
               scheduler.h DESTINATION include)
               
target_link_libraries (nest_environment
                       coreneuron10_environment)