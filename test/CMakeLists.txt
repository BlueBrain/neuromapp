
#for files that are generated automatically
include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${MPI_CXX_INCLUDE_PATH})

if(NOT Boost_USE_STATIC_LIBS)
	add_definitions(-DBOOST_TEST_DYN_LINK=TRUE)
endif()

add_subdirectory (coreneuron_1.0)
add_subdirectory (hello)
add_subdirectory (utils)
add_subdirectory (app)
add_subdirectory (keyvalue)
add_subdirectory (iobench)
add_subdirectory (hdf5)
add_subdirectory (nest)
