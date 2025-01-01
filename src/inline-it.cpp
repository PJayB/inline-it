#include <iostream>
#include <cctype>
#include <algorithm>
#include <fstream>
#include <array>
#include <iomanip>
#include <cstdint>

#include "commandline.h"

namespace {
    // Makes a string C-identifier safe.
    std::string filename_to_identifier(
        const std::string_view& filename,
        const std::string_view& prefix)
    {
        std::string out(filename);
        std::transform(
            out.begin(),
            out.end(),
            out.begin(),
            [] (const char v) -> char {
                if (!std::isalnum(v)) {
                    return '_';
                }
                else {
                    return std::toupper(v);
                }
            }
            );
        return std::string(prefix) + out;
    }

    using output_stream = std::basic_ostream<char>;
    using input_stream = std::basic_istream<char>;

    void print_bytes(
        output_stream& ostream,
        const std::string_view& chars,
        const std::string_view& prefix,
        size_t columnOffset,
        size_t columns)
    {
        static constexpr char hex_chars[] = "0123456789abcdef";

        for (size_t i = 0; i < chars.size(); ++i, ++columnOffset) {
            // If we're starting a new line, write the prefix
            if (columnOffset % columns == 0) {
                ostream << prefix;
            }

            // Write the byte
            uint8_t c = static_cast<uint8_t>(chars[i]);
            ostream << "0x" << hex_chars[(c & 0xf0) >> 4]
                << hex_chars[c & 0xf] << ", ";

            // If we're terminating a line, write a newline
            if ((columnOffset + 1) % columns == 0) {
                ostream << "\n";
            }
        }
    }

    void dump_hex(
        output_stream& ostream,
        input_stream& istream,
        const std::string_view& linePrefix,
        const std::string_view& streamSuffix)
    {
        char batch[2048]; // todo audit batch size
        constexpr size_t columns = 8; // must be factor of batch size
        size_t written = 0;
        while(istream.good()) {
            istream.read(batch, std::size(batch));
            auto r = istream.gcount();
            if (r > 0) {
                print_bytes(ostream, std::string_view(batch, r),
                    linePrefix, written % columns, columns);
                written += r;
            }
        }
        if (!istream.eof()) {
            istream.exceptions(std::ofstream::failbit);
        }
        if (streamSuffix.size() > 0) {
            print_bytes(ostream, streamSuffix, linePrefix, written % columns,
                columns);
        }
    }

    void dump_inputs(
        output_stream& ostream,
        const std::vector<std::string>& input_files,
        const std::string_view& type_mods,
        const std::string_view& id_prefix,
        bool bare,
        bool null_terminate)
    {
        // If not doing bare output, indent the values
        std::string_view linePrefix { bare ? "" : "\t" };
        std::string_view streamSuffix { null_terminate ? "\0" : "" };

        if (input_files.empty()) {
            dump_hex(ostream, std::cin, linePrefix, streamSuffix);
        }
        else {
            for (auto const& filename : input_files) {
                std::ifstream f(filename.c_str(), std::ios::binary);

                if (bare) {
                    dump_hex(ostream, f, linePrefix, streamSuffix);
                }
                else {
                    // Generate identifier from filename
                    ostream << type_mods << " "
                        << filename_to_identifier(filename, id_prefix)
                        << "[] = {" << std::endl;
                    dump_hex(ostream, f, linePrefix, streamSuffix);
                    ostream << "};\n";
                }
            }
        }
    }
}

int main(int argc, char** argv) {
    try {
        auto const options = commandline::parse(argc, argv);

        if (options.output_filename.empty()) {
            // Output to stdout if the filename is empty
            dump_inputs(std::cout, options.input_filenames, options.byte_type,
                options.prefix, options.bare, options.null_terminate);
        }
        else {
            // Output to file
            std::ofstream f(options.output_filename.c_str(),
                std::ios_base::out | std::ios_base::trunc);
            f.exceptions(std::ifstream::failbit);

            dump_inputs(f, options.input_filenames, options.byte_type,
                options.prefix, options.bare, options.null_terminate);
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
