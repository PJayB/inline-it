#include "commandline.h"

#include <functional>
#include <memory>

#include <tclap/CmdLine.h>

#ifndef PROJECT_VERSION
#   error "Please define PROJECT_VERSION"
#endif

namespace {
    class auto_tclap {
    public:
        template<typename... CArgs>
        auto_tclap(CArgs... args)
        : _CmdLine(std::forward<CArgs>(args)...) {
        }

        template<typename ValueArg, typename ResultArg,
            typename ...ConstructorArgs>
        void add(ResultArg& result, ConstructorArgs&& ...args) {
            auto p = std::make_shared<ValueArg>(
                std::forward<ConstructorArgs>(args)...);
            _CommitFuncs.push_back([p, &result] () {
                result = p->getValue();
            });
            _CmdLine.add(p.get());
        }

        void parse(int argc, char** argv) {
            _CmdLine.parse(argc, argv);
            for (auto& func : _CommitFuncs) {
                func();
            }
        }

    private:
        std::vector<std::function<void()>> _CommitFuncs;
        TCLAP::CmdLine _CmdLine;
    };

    // Returns true if a filename is a pipe from stdin or to stdout
    bool filename_is_piped(const std::string& f) {
        return f.empty() || f == "-";
    }
}

commandline::options commandline::parse(int argc, char** argv) {
    options s;
    auto_tclap cmd("inline-it", ' ', PROJECT_VERSION);

    // Switches
    cmd.add<TCLAP::SwitchArg>(s.bare, "b", "bare",
        "Just output hex, no identifiers", false);

    // Single value args
    cmd.add<TCLAP::ValueArg<std::string>>(s.output_filename, "o",
        "output-file", "The file to write to (default: stdout)", false, "-",
        "file path");
    cmd.add<TCLAP::ValueArg<std::string>>(s.prefix, "p",
        "prefix", "Prefix the identifiers", false, "",
        "string");
    cmd.add<TCLAP::ValueArg<std::string>>(s.byte_type, "t",
        "type", "The type of byte (default: static const unsigned char)", false,
        "static const unsigned char", "string");

    // The remaining args are input files
    cmd.add<TCLAP::UnlabeledMultiArg<std::string>>(s.input_filenames,
        "input-files", "The file(s) to read from (default: stdin)", false, "",
        "file path");

    cmd.parse(argc, argv);

    bool hasNamedInputs = false;
    if (!s.input_filenames.empty()) {
        // Check consistency: we either read from stdin or from files, not both.
        bool hasPipedInput = false;
        for (const auto& f : s.input_filenames) {
            if (filename_is_piped(f)) {
                hasPipedInput = true;
            }
            else {
                hasNamedInputs = true;
            }
        }

        if (hasPipedInput == hasNamedInputs) {
            throw std::runtime_error("Cannot mix input from stdin and files.");
        }
    }

    // Enable bare mode if we have no named inputs
    if (!hasNamedInputs) {
        s.input_filenames.clear();
        s.bare = true;
    }

    // Clear the output filename if it's a pipe specifier
    if (filename_is_piped(s.output_filename)) {
        s.output_filename.clear();
    }

    return s;
}
