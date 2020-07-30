set (SOURCES main.c)

add_executable(lab1.elf ${SOURCES})
target_link_libraries(lab1.elf ${330_LIBS})
set_target_properties(lab1.elf PROPERTIES LINKER_LANGUAGE CXX)
