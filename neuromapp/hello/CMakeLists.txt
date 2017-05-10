# Use -DNEUROMAPP_DISABLE_HELLO=TRUE to disable compilation/installation of this mini-app
if(NOT NEUROMAPP_DISABLE_HELLO)
    add_library (hello main.cpp)

    install (TARGETS hello DESTINATION lib)
    install (FILES hello.h DESTINATION include)

    set(NEUROMAPP_HELLO_MAPP ON CACHE BOOL "True if hello mini-app will be installed")
else()
    set(NEUROMAPP_HELLO_MAPP OFF CACHE BOOL "True if hello mini-app will be installed")
endif()
