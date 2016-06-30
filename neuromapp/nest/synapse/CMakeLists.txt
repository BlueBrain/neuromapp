add_library (synapse connectionmanager.cpp connector_base.cpp event.cpp node.cpp scheduler.cpp drivers/model_main.cpp)
target_link_libraries(synapse coreneuron10_environment)

install (TARGETS synapse DESTINATION lib)
install (FILES connector_base.h event.h node.h scheduler.h drivers/synapse.h DESTINATION include)

