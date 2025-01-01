cmake_minimum_required(VERSION 3.27)

cmake_policy(SET CMP0156 NEW)

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif ()

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_WIN32_EXECUTABLE OFF)

project(inline-it
    VERSION 0.3
    LANGUAGES C CXX
    HOMEPAGE_URL "https://github.com/PJayB/inline-it"
    )

add_subdirectory(extra)
add_subdirectory(src)
