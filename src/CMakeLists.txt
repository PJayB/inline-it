add_executable(inline-it
    inline-it.cpp
)

target_link_libraries(inline-it PRIVATE
    extras_fmt
    extras_tclap
)
