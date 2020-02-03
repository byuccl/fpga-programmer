file(GLOB cSources *.c)
file(GLOB cxxSources *.cpp)

# set(CMAKE_C_FLAGS, "-mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard" CACHE STRING "" FORCE)
# set(CMAKE_CXX_FLAGS, "-mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard" CACHE STRING "" FORCE)


add_library(supportFiles ${cSources} ${cxxSources})


