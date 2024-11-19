#pragma once

#include <vector>
#include <string>

namespace commandline {
    struct options {
        /*
        Examples of options we might want in the future:

        - Trailing comma or not
        - Uppercase hex
        - Spaces instead of tabs, customizable indent
        - Column count
        - Variable case/style, e.g. camel, sentence, snake, etc.
        - Explicit length constant, xxd-style
        - Add a trailing null terminator
        - Output in decimal
        - Output in other widths than byte, e.g. qword or dword

        */
        std::vector<std::string> input_filenames; // empty = stdin
        std::string output_filename; // empty = stdout
        std::string prefix; // for the constant names, e.g. "k_" -> "k_foo"
        std::string byte_type; // the type of char, e.g. unsigned char, uint8_t, etc.
        bool bare;
    };

    options parse(int argc, char** argv);
}
