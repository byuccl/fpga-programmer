add_executable(lasertag.elf
main.c
filterTest.c
histogram.c
# filter.c 
# queue_test.c
# sound.c
# timer_ps.c
# runningModes.c
)

add_subdirectory(sounds)
target_link_libraries(lasertag.elf ${330_LIBS} sounds lasertag_libs queue_lib)
set_target_properties(lasertag.elf PROPERTIES LINKER_LANGUAGE CXX)
