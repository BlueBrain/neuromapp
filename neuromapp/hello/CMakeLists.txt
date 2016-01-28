add_library (hello main.cpp)

install (TARGETS hello DESTINATION lib)
install (FILES hello.h DESTINATION include)
