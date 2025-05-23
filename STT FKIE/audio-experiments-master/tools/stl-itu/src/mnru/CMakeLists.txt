include_directories(../utl)

add_executable(mnrudemo mnrudemo.c mnru.c ../utl/ugst-utl.c filtering_routines.c)
target_link_libraries(mnrudemo ${M_LIBRARY})

add_executable(p50fbmnru p50fbmnru.c mnru.c ../utl/ugst-utl.c filtering_routines.c )
target_link_libraries(p50fbmnru ${M_LIBRARY})

add_executable(snr calc-snr.c)
target_link_libraries(snr ${M_LIBRARY})

#TEST: MNRU
add_test(mnrudemo1 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mnrudemo -q test_data/sine.src test_data/sine.q00 256 1 20 00)
add_test(mnrudemo1-verify ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cf -q test_data/sine-q00.unx test_data/sine.q00)

add_test(mnrudemo2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mnrudemo -q test_data/sine.src test_data/sine.q05 256 1 20 05)
add_test(mnrudemo2-verify ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cf -q test_data/sine-q05.unx test_data/sine.q05)

add_test(mnrudemo3 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mnrudemo -q test_data/sine.src test_data/sine.q10 256 1 20 10)
add_test(mnrudemo3-verify ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cf -q test_data/sine-q10.unx test_data/sine.q10)

add_test(mnrudemo4 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mnrudemo -q test_data/sine.src test_data/sine.q15 256 1 20 15)
add_test(mnrudemo4-verify ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cf -q test_data/sine-q15.unx test_data/sine.q15)

add_test(mnrudemo5 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mnrudemo -q test_data/sine.src test_data/sine.q20 256 1 20 20)
add_test(mnrudemo5-verify ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cf -q test_data/sine-q20.unx test_data/sine.q20)

add_test(mnrudemo6 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mnrudemo -q test_data/sine.src test_data/sine.q25 256 1 20 25)
add_test(mnrudemo6-verify ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cf -q test_data/sine-q25.unx test_data/sine.q25)

add_test(mnrudemo7 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mnrudemo -q test_data/sine.src test_data/sine.q30 256 1 20 30)
add_test(mnrudemo7-verify ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cf -q test_data/sine-q30.unx test_data/sine.q30)

add_test(mnrudemo8 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mnrudemo -q test_data/sine.src test_data/sine.q35 256 1 20 35)
add_test(mnrudemo8-verify ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cf -q test_data/sine-q35.unx test_data/sine.q35)

add_test(mnrudemo9 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mnrudemo -q test_data/sine.src test_data/sine.q40 256 1 20 40)
add_test(mnrudemo9-verify ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cf -q test_data/sine-q40.unx test_data/sine.q40)

add_test(mnrudemo10 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mnrudemo -q test_data/sine.src test_data/sine.q45 256 1 20 45)
add_test(mnrudemo10-verify ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cf -q test_data/sine-q45.unx test_data/sine.q45)

add_test(mnrudemo11 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mnrudemo -q test_data/sine.src test_data/sine.q50 256 1 20 50)
add_test(mnrudemo11-verify ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cf -q test_data/sine-q50.unx test_data/sine.q50)

add_test(mnrudemo12 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mnrudemo -q test_data/sine.src test_data/sine.q99 256 1 20 150)
add_test(mnrudemo12-verify ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cf -q test_data/sine-q99.unx test_data/sine.q99)


#TEST: P50 FB MNRU
add_test(p50fbmnru_Q20_legacyDC ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/p50fbmnru test_data/P501_D_AM_fm_FB_48k.pcm test_data/P501_D_AM_fm_FB_48k_Q20.pcm 20 M 1)
add_test(p50fbmnru_Q20_legacyDC-verify ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cf -q test_data/P501_D_AM_fm_FB_48k_Q20.ref test_data/P501_D_AM_fm_FB_48k_Q20.pcm)

add_test(p50fbmnru-Q10_legacyDC ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/p50fbmnru test_data/P501_D_AM_fm_FB_48k.pcm test_data/P501_D_AM_fm_FB_48k_Q10.pcm 10 M 1)
add_test(p50fbmnru-Q10_legacyDC-verify ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cf -q test_data/P501_D_AM_fm_FB_48k_Q10.ref test_data/P501_D_AM_fm_FB_48k_Q10.pcm)

add_test(p50fbmnru-Q20_legacyDC_noise ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/p50fbmnru test_data/P501_D_AM_fm_FB_48k.pcm test_data/P501_D_AM_fm_FB_48k_Q20_noise.pcm 20 N 1)
add_test(p50fbmnru-Q20_legacyDC_noise-verify ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cf -q test_data/P501_D_AM_fm_FB_48k_Q20_noise.ref test_data/P501_D_AM_fm_FB_48k_Q20_noise.pcm)

add_test(p50fbmnru-Q10_legacyDC_noise ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/p50fbmnru test_data/P501_D_AM_fm_FB_48k.pcm test_data/P501_D_AM_fm_FB_48k_Q10_noise.pcm 10 N 1)
add_test(p50fbmnru-Q10_legacyDC_noise-verify ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cf -q test_data/P501_D_AM_fm_FB_48k_Q10_noise.ref test_data/P501_D_AM_fm_FB_48k_Q10_noise.pcm)

add_test(p50fbmnru-Q20_legacyDC_2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/p50fbmnru test_data/P501_D_EN_fm_SWB_48k.pcm test_data/P501_D_EN_fm_SWB_48k_Q20.pcm 20 M 1)
add_test(p50fbmnru-Q20_legacyDC_2-verify ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cf -q test_data/P501_D_EN_fm_SWB_48k_Q20.ref test_data/P501_D_EN_fm_SWB_48k_Q20.pcm)

add_test(p50fbmnru-Q10_legacyDC_2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/p50fbmnru test_data/P501_D_EN_fm_SWB_48k.pcm test_data/P501_D_EN_fm_SWB_48k_Q10.pcm 10 M 1)
add_test(p50fbmnru-Q10_legacyDC_2-verify ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cf -q test_data/P501_D_EN_fm_SWB_48k_Q10.ref test_data/P501_D_EN_fm_SWB_48k_Q10.pcm)

add_test(p50fbmnru-Q20_legacyDC_noise_2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/p50fbmnru test_data/P501_D_EN_fm_SWB_48k.pcm test_data/P501_D_EN_fm_SWB_48k_Q20_noise.pcm 20 N 1)
add_test(p50fbmnru-Q20_legacyDC_noise_2-verify ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cf -q test_data/P501_D_EN_fm_SWB_48k_Q20_noise.ref test_data/P501_D_EN_fm_SWB_48k_Q20_noise.pcm)

add_test(p50fbmnru-Q10_legacyDC_noise_2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/p50fbmnru test_data/P501_D_EN_fm_SWB_48k.pcm test_data/P501_D_EN_fm_SWB_48k_Q10_noise.pcm 10 N 1)
add_test(p50fbmnru-Q10_legacyDC_noise_2-verify ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cf -q test_data/P501_D_EN_fm_SWB_48k_Q10_noise.ref test_data/P501_D_EN_fm_SWB_48k_Q10_noise.pcm)

add_test(p50fbmnru_Q20_noDcFilter ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/p50fbmnru test_data/P501_D_AM_fm_FB_48k.pcm test_data/P501_D_AM_fm_FB_48k_Q20_noDCFilter.pcm 20 M 0)
add_test(p50fbmnru_Q20_noDcFilter-verify ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cf -q test_data/P501_D_AM_fm_FB_48k_Q20_noDCFilter.ref test_data/P501_D_AM_fm_FB_48k_Q20_noDCFilter.pcm)

add_test(p50fbmnru_Q20_DC60Hz ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/p50fbmnru test_data/P501_D_AM_fm_FB_48k.pcm test_data/P501_D_AM_fm_FB_48k_Q20_DC60Hz.pcm 20 M 2)
add_test(p50fbmnru_Q20_DC60Hz-verify ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cf -q test_data/P501_D_AM_fm_FB_48k_Q20_DC60Hz.ref test_data/P501_D_AM_fm_FB_48k_Q20_DC60Hz.pcm)

add_test(p50fbmnru_Q20_DC30Hz ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/p50fbmnru test_data/P501_D_AM_fm_FB_48k.pcm test_data/P501_D_AM_fm_FB_48k_Q20_DC30Hz.pcm 20 M 3)
add_test(p50fbmnru_Q20_DC30Hz-verify ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cf -q test_data/P501_D_AM_fm_FB_48k_Q20_DC30Hz.ref test_data/P501_D_AM_fm_FB_48k_Q20_DC30Hz.pcm)

add_test(p50fbmnru_Q20_DC15Hz ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/p50fbmnru test_data/P501_D_AM_fm_FB_48k.pcm test_data/P501_D_AM_fm_FB_48k_Q20_DC15Hz.pcm 20 M 4)
add_test(p50fbmnru_Q20_DC15Hz-verify ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cf -q test_data/P501_D_AM_fm_FB_48k_Q20_DC15Hz.ref test_data/P501_D_AM_fm_FB_48k_Q20_DC15Hz.pcm)

add_test(p50fbmnru_Q0_clipping_clamp ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/p50fbmnru test_data/P501_D_AM_fm_FB_48k.pcm test_data/P501_D_AM_fm_FB_48k_Q0_clamp.pcm 0 M 4)
add_test(p50fbmnru_Q0_clipping_clamp-verify ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cf -q test_data/P501_D_AM_fm_FB_48k_Q0_clamp.ref test_data/P501_D_AM_fm_FB_48k_Q0_clamp.pcm)

add_test(p50fbmnru_Q0_clipping_overflow ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/p50fbmnru test_data/P501_D_AM_fm_FB_48k.pcm test_data/P501_D_AM_fm_FB_48k_Q0_overflow.pcm 0 M --overflow)
add_test(p50fbmnru_Q0_clipping_overflow-verify ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cf -q test_data/P501_D_AM_fm_FB_48k_Q0_overflow.ref test_data/P501_D_AM_fm_FB_48k_Q0_overflow.pcm)

#TEST: Compute SNR for MNRU files
#TODO: no automatic verification data available
add_test(snr1 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/snr -q test_data/sine.q99 test_data/sine.q00)
add_test(snr2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/snr -q test_data/sine.q99 test_data/sine.q05)
add_test(snr3 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/snr -q test_data/sine.q99 test_data/sine.q10)
add_test(snr4 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/snr -q test_data/sine.q99 test_data/sine.q15)
add_test(snr5 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/snr -q test_data/sine.q99 test_data/sine.q20)
add_test(snr6 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/snr -q test_data/sine.q99 test_data/sine.q25)
add_test(snr7 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/snr -q test_data/sine.q99 test_data/sine.q30)
add_test(snr8 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/snr -q test_data/sine.q99 test_data/sine.q35)
add_test(snr9 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/snr -q test_data/sine.q99 test_data/sine.q40)
add_test(snr10 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/snr -q test_data/sine.q99 test_data/sine.q45)
add_test(snr11 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/snr -q test_data/sine.q99 test_data/sine.q50)

