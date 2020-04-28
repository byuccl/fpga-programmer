file(GLOB cSources *.c)
file(GLOB cxxSources *.cpp)

add_library(supportFiles ${cSources} ${cxxSources})