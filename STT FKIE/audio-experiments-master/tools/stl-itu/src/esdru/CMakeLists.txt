include_directories(../utl)

add_executable(esdru esdru.c ../utl/ugst-utl.c)
target_link_libraries(esdru ${M_LIBRARY})


#TEST: ESDRU
add_test(esdru1 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/esdru -e_out test_data/es_el.test.double 0.2 test_data/stereo_test.pcm test_data/stereo_test.0.2.test_dummy.pcm)
add_test(esdru1-verify ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cf -q -reltol 1e-6 test_data/es_el.test.double test_data/es_el.double)
add_test(esdru2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/esdru -e_in test_data/es_el.double 0.2 test_data/stereo_test.pcm test_data/stereo_test.0.2.test.pcm)
add_test(esdru2-verify ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/signal-diff -q -equiv 2 test_data/stereo_test.0.2.test.pcm test_data/stereo_test.0.2.pcm)
add_test(esdru3 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/esdru 1.0 test_data/stereo_test.pcm test_data/stereo_test.1.0.test.pcm)
add_test(esdru3-verify ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cf -q test_data/stereo_test.pcm test_data/stereo_test.1.0.test.pcm)

