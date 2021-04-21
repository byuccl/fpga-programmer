
SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_PROCESSOR armv7)
SET(CMAKE_CROSSCOMPILING 1)

set(CMAKE_C_COMPILER "arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "arm-none-eabi-g++")
SET(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS)
SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)

add_compile_options("-march=armv7-a")
add_compile_options("-mfpu=vfpv3")
add_compile_options("-mfloat-abi=hard")
add_compile_options("-O2")

add_link_options("-march=armv7-a")
add_link_options("-mfpu=vfpv3")
add_link_options("-mfloat-abi=hard")
add_link_options("-mhard-float")
add_link_options("-Wl,-build-id=none")
add_link_options(-specs=${CMAKE_CURRENT_LIST_DIR}/Xilinx.spec)
add_link_options(-Wl,-T)
add_link_options(-Wl,${CMAKE_CURRENT_LIST_DIR}/lscript.ld)

