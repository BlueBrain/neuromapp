# Use -DNEUROMAPP_DISABLE_READI=TRUE to disable compilation/installation of this mini-app
if(NOT NEUROMAPP_DISABLE_READI)
    add_definitions("-std=c++11")
    add_library (readi main.cpp)
    install (TARGETS readi DESTINATION lib)
    install (FILES readi.h DESTINATION include)

    set(NEUROMAPP_READI_MAPP ON CACHE BOOL "True if readi mini-app will be installed")
else()
    set(NEUROMAPP_READI_MAPP OFF CACHE BOOL "True if readi mini-app will be installed")
endif()
