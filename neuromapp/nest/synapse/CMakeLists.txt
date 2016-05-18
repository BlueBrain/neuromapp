add_library (synapse connector_base.cpp event.cpp node.cpp main.cpp)

install (TARGETS synapse DESTINATION lib)
install (FILES connector_base.h event.h node.h scheduler.h synapse.h DESTINATION include)
