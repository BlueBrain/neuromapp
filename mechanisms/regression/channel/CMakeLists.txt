add_definitions (-DBOOST_TEST_DYN_LINK -Wno-unused-variable)

set(tests Na SKv3_1 NaTs2_t)

if(${SIMD_TECH} STREQUAL "avx" OR ${SIMD_TECH} STREQUAL "sse" )
    set(simd_test sse avx)
endif()

if(${SIMD_TECH} STREQUAL "qpx")
    set(simd_test qpx)
endif()

if(${SIMD_TECH} STREQUAL "mic")
    set(simd_test mic)
endif()

set(SOURCES_MEMORY ${PROJECT_SOURCE_DIR}/original_version/channel)

foreach(test ${tests})
    foreach(simd ${simd_test})
        add_executable(${test}_test_states_rates_${simd} test_states_rates.cpp ${SOURCES_MEMORY}/${test}.cpp)
        SET_TARGET_PROPERTIES(${test}_test_states_rates_${simd} PROPERTIES COMPILE_FLAGS "-m${simd} -D__CYME_SIMD_VALUE__=${simd} -D__MECHANISM__CHANNEL_=${test}")
        target_link_libraries(${test}_test_states_rates_${simd} ${Boost_LIBRARIES})
        add_test(${test}_test_states_rates_${simd} ${test}_test_states_rates_${simd})
    endforeach(simd)
endforeach(test)
