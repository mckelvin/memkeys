file(GLOB_RECURSE memkeys_HEADERS *.h)
file(GLOB_RECURSE memkeys_core_SOURCES *.cpp)
list(REMOVE_ITEM memkeys_core_SOURCES main.cpp)

add_library(memkeys-core STATIC ${memkeys_HEADERS} ${memkeys_core_SOURCES})
target_link_libraries(memkeys-core pcap pthread pcrecpp ncurses)

add_executable(memkeys main.cpp)
target_link_libraries(memkeys memkeys-core)
install(TARGETS memkeys RUNTIME DESTINATION bin)
