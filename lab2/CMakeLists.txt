set (SOURCES main.c)
set (MY_LIBS buttons_switches)

add_executable(lab2.elf ${SOURCES})
target_link_libraries(lab2.elf ${330_LIBS} ${MY_LIBS})
set_target_properties(lab2.elf PROPERTIES LINKER_LANGUAGE CXX)