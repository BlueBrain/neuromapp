include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${HDF5_INCLUDE_DIRS})

add_library (h5import H5Synapses.cpp h5reader.cpp fakenestkernel/nest_kernel.cpp)
target_link_libraries(h5import ${HDF5_LIBRARIES})

install (TARGETS h5import DESTINATION lib)
install (FILES H5Synapses.h h5reader.h kernels.h SynapseList.h fakenestkernel/nest_kernel.h DESTINATION include)
