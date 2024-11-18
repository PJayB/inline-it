#pragma once

#include <vector>
#include <string>

namespace commandline {
    struct options {
        std::vector<std::string> input_filenames; // empty = stdin
        std::string output_filename; // empty = stdout
        std::string prefix; // for the constant names, e.g. "k_" -> "k_foo"
        std::string byte_type; // the type of char, e.g. unsigned char, uint8_t, etc.
        bool bare;
    };

    options parse(int argc, char** argv);
}
