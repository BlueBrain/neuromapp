add_definitions("-mavx  -D__CYME_SIMD_VALUE__=${SIMD_TECH}")

include_directories ("${PROJECT_SOURCE_DIR}/cyme/include/cyme")
include_directories ("${PROJECT_SOURCE_DIR}/cyme_version")

set(SOURCES_MEMORY ${PROJECT_SOURCE_DIR}/original_version/channel/Na.cpp)

set(LIBRAIRIES ${MASS_LIBRARY} ${SIMD_MASS_LIBRARY})

add_executable(main_cyme main.cpp  ${SOURCES_MEMORY})

if(CYME_MATH)
    target_link_libraries(main_cyme ${LIBRAIRIES} ${Boost_CHRONO_LIBRARY}
${Boost_SYSTEM_LIBRARY}  )
else(CYME_MATH)
    target_link_libraries(main_cyme  ${Boost_CHRONO_LIBRARY}
${Boost_SYSTEM_LIBRARY}  )
endif(CYME_MATH)
