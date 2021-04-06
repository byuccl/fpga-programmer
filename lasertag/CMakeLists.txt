add_executable(lasertag.elf
main.c
filter_solns.c
filterTest.c
histogram.c
sound.c
timer_ps.c
# runningModes.c
)

add_subdirectory(sounds)
#add_subdirectory(bluetooth) # Optional code for the creative project.
target_link_libraries(lasertag.elf ${330_LIBS} sounds lasertag_libs queue_lib)
set_target_properties(lasertag.elf PROPERTIES LINKER_LANGUAGE CXX)
