add_executable(lab3.elf main.c)
target_link_libraries(lab3.elf ${330_LIBS} intervalTimer buttons_switches)
set_target_properties(lab3.elf PROPERTIES LINKER_LANGUAGE CXX)