add_executable(lab2.elf main.c)
target_link_libraries(lab2.elf ${330_LIBS} buttons_switches)
set_target_properties(lab2.elf PROPERTIES LINKER_LANGUAGE CXX)