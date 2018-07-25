
#for files that are generated automatically
include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${MPI_CXX_INCLUDE_PATH})

if(NOT Boost_USE_STATIC_LIBS)
	add_definitions(-DBOOST_TEST_DYN_LINK=TRUE)
endif()

add_subdirectory (utils)
add_subdirectory (app)

if(NEUROMAPP_CORENEURON_MAPP)
    add_subdirectory (coreneuron_1.0)
endif()

if(NEUROMAPP_HELLO_MAPP)
    add_subdirectory (hello)
endif()

if(NEUROMAPP_IOBENCH_MAPP)
    add_subdirectory (iobench)
endif()

if(NEUROMAPP_KEYVALUE_MAPP)
    add_subdirectory (keyvalue)
endif()

if(NEUROMAPP_HDF5_MAPP)
    add_subdirectory (hdf5)
endif()

if(NEUROMAPP_NEST_MAPP)
    add_subdirectory (nest)
endif()

if(NEUROMAPP_REPLIB_MAPP)
    add_subdirectory (replib)
endif()

if(NEUROMAPP_READI_MAPP)
    add_subdirectory (readi)

if(NEUROMAPP_COMPRESSION_MAPP)
    add_subdirectory(compression)
endif()
