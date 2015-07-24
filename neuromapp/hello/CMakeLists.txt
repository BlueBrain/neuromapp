add_library (hello main.cpp)

add_subdirectory (test)

install (TARGETS hello DESTINATION lib)
install (FILES hello.h DESTINATION include)
