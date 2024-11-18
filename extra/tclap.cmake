add_library(extras_tclap INTERFACE)
target_include_directories(extras_tclap
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/tclap/include
    )
