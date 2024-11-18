add_executable(inline-it
    commandline.h
    commandline.cpp
    inline-it.cpp
)

target_link_libraries(inline-it PRIVATE
    extras_fmt
    extras_tclap
)
