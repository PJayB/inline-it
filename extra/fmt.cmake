add_library(extras_fmt INTERFACE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "build static lib" FORCE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "build static lib" FORCE)
add_subdirectory(fmt)
target_include_directories(extras_fmt INTERFACE fmt/include)
target_link_libraries(extras_fmt INTERFACE fmt)
