add_library (synapse event.cpp node.cpp main.cpp)

install (TARGETS synapse DESTINATION lib)
install (FILES synapse.h DESTINATION include)
