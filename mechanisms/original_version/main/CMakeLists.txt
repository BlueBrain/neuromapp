########################################################################
#
# Compile and prepare main
#
########################################################################
include_directories ("${PROJECT_SOURCE_DIR}/original_version/memory")

set(SOURCES_MEMORY ${PROJECT_SOURCE_DIR}/original_version/memory/cxprop.cpp
                   ${PROJECT_SOURCE_DIR}/original_version/memory/mechanism.c
                   ${PROJECT_SOURCE_DIR}/original_version/memory/symbol.c
                   ${PROJECT_SOURCE_DIR}/original_version/channel/channel_Na/channel_na.cpp)

add_executable(main_corebluron main.cpp ${SOURCES_MEMORY})
target_link_libraries(main_corebluron ${Boost_CHRONO_LIBRARY}
${Boost_SYSTEM_LIBRARY})
 
