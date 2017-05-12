## using the hello tool as a template for the initial cmake instructions
# Use -DNEUROMAPP_DISABLE_comp_test=TRUE to disable compilation/installation of this mini-app
if(NOT NEUROMAPP_DISABLE_COMP_TEST)
    add_library (comp_test main.cpp)

    install (TARGETS comp_test DESTINATION lib)
    install (FILES comp_test.h DESTINATION include)

    set(NEUROMAPP_COMP_TEST_MAPP ON CACHE BOOL "True if comp_test mini-app will be installed")
else()
    set(NEUROMAPP_COMP_TEST_MAPP OFF CACHE BOOL "True if comp_test mini-app will be installed")
endif()


