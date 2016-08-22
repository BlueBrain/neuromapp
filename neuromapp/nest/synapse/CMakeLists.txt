include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})

add_library (synapse connectionmanager.cpp connector_base.cpp event.cpp node.cpp scheduler.cpp drivers/model_main.cpp)
target_link_libraries(synapse coreneuron10_environment)

install (TARGETS synapse DESTINATION lib)
install (FILES connector_base.h event.h node.h scheduler.h drivers/synapse.h DESTINATION include)

#NEST manager libraray
add_library (nest_manager connectionmanager.cpp
                          connector_base.cpp
                          event.cpp
                          node.cpp
                          scheduler.cpp)

install (TARGETS nest_manager DESTINATION lib)
install (FILES connectionmanager.h
               connector_base.h
               event.h
               node.h
               scheduler.h DESTINATION include)
               
target_link_libraries (nest_manager
                       coreneuron10_queueing
                       coreneuron10_environment)