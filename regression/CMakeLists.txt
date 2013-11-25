set(MINIAPPS_LIBRARIES ${Boost_LIBRARIES})

include_directories ("${PROJECT_SOURCE_DIR}/cyme_version")
include_directories ("${PROJECT_SOURCE_DIR}/cyme/include/cyme")

add_subdirectory (channel)
add_subdirectory (synapse)
