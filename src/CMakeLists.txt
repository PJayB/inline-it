add_executable(inline-it
    commandline.h
    commandline.cpp
    inline-it.cpp
)

target_compile_definitions(inline-it PRIVATE
    PROJECT_VERSION="${CMAKE_PROJECT_VERSION}"
)

target_link_libraries(inline-it PRIVATE
    extras_tclap
)

install(
    TARGETS inline-it
)
