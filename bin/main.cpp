#include <iostream>
#include <string>

#include "lib/DiffPrint/DiffPrint.h"
#include "lib/Tokenizer/Tokenizer.h"
#include "lib/argparse/argparse.hpp"
#include "lib/DiffRunner/DiffRunner.h"

using namespace std::literals::string_literals;

int main(int argc, char* argv[]) {
    argparse::ArgumentParser args("diff");
    args.add_argument("old").help("old file path");
    args.add_argument("new").help("new file path");
    args.add_argument("--parser", "-p")
        .default_value("utf-8"s)
        .help("parser mode: bytes, utf-8")
        .metavar("PARSER_MODE");
    args.add_argument("--tokenizer", "-t")
        .default_value("line"s)
        .help(
            "tokenizer type: symbol, word, line, ignore-all-space, ignore-space-change, "
            "semantic-code")
        .metavar("TOKENIZER_TYPE");
    args.add_argument("--common-prefix")
        .default_value(""s)
        .help("prefix for printing common part in raw mode");
    args.add_argument("--old-prefix")
        .default_value("[-]"s)
        .help("prefix for printing deleted part in raw mode");
    args.add_argument("--new-prefix")
        .default_value("[+]"s)
        .help("prefix for printing added part in raw mode");
    args.add_argument("--benchmark", "-b")
        .help("measure timings")
        .default_value(false)
        .implicit_value(true);
    args.add_argument("--common", "-c")
        .help("print common parts of the files")
        .default_value(false)
        .implicit_value(true);
    args.add_argument("--raw")
        .help("print diff without colors")
        .default_value(false)
        .implicit_value(true);
    args.add_argument("--show-common-newlines")
        .help("print common part with newlines displayed as '\\n'")
        .default_value(false)
        .implicit_value(true);
    args.add_argument("--show-pos")
        .help("print diff start position in both files")
        .default_value(false)
        .implicit_value(true);
    args.add_argument("--recursive", "-r")
        .help("recursive directory mode")
        .default_value(false)
        .implicit_value(true);

    try {
        args.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << args;
        return 1;
    }

    auto old_file_path = args.get<std::string>("old");
    auto new_file_path = args.get<std::string>("new");
    auto tokeninzer_arg = args.get<std::string>("--tokenizer");
    auto parser_arg = args.get<std::string>("--parser");
    auto benchmark = args.get<bool>("--benchmark");
    auto recursive = args.get<bool>("--recursive");

    TokenizerMode tokenizer_mode;
    if (tokeninzer_arg == "symbol") {
        tokenizer_mode = TokenizerMode::SYMBOL;
    } else if (tokeninzer_arg == "word") {
        tokenizer_mode = TokenizerMode::WORD;
    } else if (tokeninzer_arg == "line") {
        tokenizer_mode = TokenizerMode::LINE;
    } else if (tokeninzer_arg == "ignore-all-space") {
        tokenizer_mode = TokenizerMode::IGNORE_ALL_SPACE;
    } else if (tokeninzer_arg == "ignore-space-change") {
        tokenizer_mode = TokenizerMode::IGNORE_SPACE_CHANGE;
    } else if (tokeninzer_arg == "semantic-code") {
        tokenizer_mode = TokenizerMode::SEMANTIC_CODE;
    } else {
        std::cerr << "Invalid tokenizer mode" << std::endl;
        std::cerr << args << std::endl;
        return 1;
    }

    ParserMode parser_mode;
    if (parser_arg == "bytes") {
        parser_mode = ParserMode::BYTES;
    } else if (parser_arg == "utf-8") {
        parser_mode = ParserMode::UTF_8;
    } else {
        std::cerr << "Invalid parser mode" << std::endl;
        std::cerr << args << std::endl;
        return 1;
    }

    DiffPrint::DiffPrinter::Mode mode{
        .raw = args.get<bool>("--raw"),
        .common_part = args.get<bool>("--common"),
        .show_common_newlines = args.get<bool>("--show-common-newlines"),
        .show_pos = args.get<bool>("--show-pos"),
        .old_prefix = args.get<std::string>("--old-prefix"),
        .new_prefix = args.get<std::string>("--new-prefix"),
        .common_prefix = args.get<std::string>("--common-prefix"),
    };
    DiffPrint::DiffPrinter diff_printer{.mode = mode};

    DiffRunner diff_runner(tokenizer_mode, parser_mode, diff_printer, benchmark);

    try {
        if (!recursive) {
            diff_runner.FileDiff(std::cout, old_file_path, new_file_path);
        } else {
            diff_runner.DirDiff(std::cout, old_file_path, new_file_path);
        }

        if (benchmark) {
            std::cerr << "mmap: " << diff_runner.GetMmapMs() << "ms\n"
                      << "tokenize: " << diff_runner.GetTokenizeMs() << "ms\n"
                      << "diff: " << diff_runner.GetDiffMs() << " ms\n"
                      << "print: " << diff_runner.GetPrintMs() << " ms\n";
        }
    } catch (std::exception& ex) {
        std::cerr << ex.what() << '\n';
        return 1;
    }
    return 0;
}
