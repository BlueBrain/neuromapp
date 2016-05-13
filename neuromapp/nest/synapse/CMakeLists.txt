add_library (synapse main.cpp)

install (TARGETS synapse DESTINATION lib)
install (FILES synapse.h DESTINATION include)
