add_library (synapse connector_base.cpp event.cpp node.cpp scheduler.cpp drivers/model_main.cpp drivers/connector_main.cpp)

install (TARGETS synapse DESTINATION lib)
install (FILES connector_base.h event.h node.h scheduler.h drivers/synapse.h DESTINATION include)

