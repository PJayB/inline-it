#include <iostream>
#include <cctype>
#include <algorithm>
#include <fstream>
#include <array>
#include <span>
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

    // Returns a separator appropriate for a byte's position within a row.
    inline const char* list_separator(
        size_t column_offset,
        size_t columns)
    {
        if (column_offset % columns == 0) {
            return ",\n";
        }
        else {
            return ", ";
        }
    }

    // Writes a comma-separated list of bytes as hex. The bytes are split at
    // every 'columns' columns, except the last byte, which never has a
    // trailing comma or new-line.
    void print_bytes(
        output_stream& ostream,
        const std::span<uint8_t>& chars,
        const std::string_view& prefix,
        size_t bytes_written,
        size_t columns)
    {
        static constexpr char hex_chars[] = "0123456789abcdef";

        // If we've already written some data, separate the new data
        // with either a new line or a comma
        if (bytes_written > 0) {
            ostream << list_separator(bytes_written, columns);
        }

        for (size_t i = 0; i < chars.size(); ++i, ++bytes_written) {
            // If we're starting a new line, write the prefix
            if (bytes_written % columns == 0) {
                ostream << prefix;
            }

            // Write the byte
            uint8_t c = static_cast<uint8_t>(chars[i]);
            ostream << "0x" << hex_chars[(c & 0xf0) >> 4]
                << hex_chars[c & 0xf];

            // Write the separator for all but the last byte
            if ((i + 1) < chars.size()) {
                ostream << list_separator(bytes_written + 1, columns);
            }
        }
    }

    void dump_hex(
        output_stream& ostream,
        input_stream& istream,
        const std::string_view& linePrefix,
        bool null_terminate)
    {
        std::array<uint8_t, 2048> batch; // todo audit batch size
        constexpr size_t columns = 8; // must be factor of batch size
        size_t bytes_written = 0;
        while(istream.good()) {
            istream.read(reinterpret_cast<char*>(batch.data()), batch.size());
            auto r = istream.gcount();
            if (r > 0) {
                // Write this block of bytes
                print_bytes(ostream, std::span {
                        batch.begin(),
                        batch.begin() + r
                    }, linePrefix, bytes_written, columns);
                bytes_written += r;
            }
        }
        if (!istream.eof()) {
            istream.exceptions(std::ofstream::failbit);
        }
        if (null_terminate) {
            std::array<uint8_t, 1> null { 0 };
            print_bytes(ostream, std::span { null }, linePrefix, bytes_written,
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
        std::string_view line_prefix { bare ? "" : "\t" };

        if (input_files.empty()) {
            dump_hex(ostream, std::cin, line_prefix, null_terminate);
        }
        else {
            for (auto const& filename : input_files) {
                std::ifstream f(filename.c_str(), std::ios::binary);

                if (bare) {
                    dump_hex(ostream, f, line_prefix, null_terminate);
                }
                else {
                    // Generate identifier from filename
                    ostream << type_mods << " "
                        << filename_to_identifier(filename, id_prefix)
                        << "[] = {" << std::endl;
                    dump_hex(ostream, f, line_prefix, null_terminate);
                    ostream << "\n};\n";
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
