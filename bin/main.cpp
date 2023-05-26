#include <sys/fcntl.h>
#include <sys/mman.h>

#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include "lib/DiffPrint/DiffPrint.h"
#include "lib/MyersDiff/MyersDiff.h"
#include "lib/Timer/Timer.h"
#include "lib/Tokenizer/Tokenizer.h"
#include "lib/argparse/argparse.hpp"

const char* MmapFile(const char* file_path, size_t file_size) {
    int fd = open(file_path, O_RDONLY);
    errno = 0;
    void* mem = mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mem == MAP_FAILED) {
        throw std::runtime_error("mmap: " + std::string(std::strerror(errno)));
    }
    return static_cast<const char*>(mem);
}

static Timer timer;

int main(int argc, char* argv[]) {
    argparse::ArgumentParser args("diff");
    args.add_argument("old").help("old file path");
    args.add_argument("new").help("new file path");
    args.add_argument("--parser", "-p")
        .default_value(std::string{"utf-8"})
        .help("parser mode: bytes, utf-8")
        .metavar("PARSER_MODE");
    args.add_argument("--tokenizer", "-t")
        .default_value(std::string{"line"})
        .help(
            "tokenizer type: symbol, word, line, ignore-all-space, ignore-space-change, "
            "semantic-code")
        .metavar("TOKENIZER_TYPE");
    args.add_argument("--common-prefix")
        .default_value(std::string{""})
        .help("prefix for printing common part in raw mode");
    args.add_argument("--old-prefix")
        .default_value(std::string{"[-]"})
        .help("prefix for printing deleted part in raw mode");
    args.add_argument("--new-prefix")
        .default_value(std::string{"[+]"})
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

    if (benchmark) {
        timer.Start();
    }

    const char* old_file_mem;
    size_t old_file_size;
    const char* new_file_mem;
    size_t new_file_size;
    try {
        old_file_size = std::filesystem::file_size(old_file_path);
        old_file_mem = MmapFile(old_file_path.c_str(), old_file_size);
        new_file_size = std::filesystem::file_size(new_file_path);
        new_file_mem = MmapFile(new_file_path.c_str(), new_file_size);
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        std::cerr << args << std::endl;
        return 1;
    }
    std::string_view old_file(old_file_mem, old_file_size);
    std::string_view new_file(new_file_mem, new_file_size);

    if (benchmark) {
        timer.Duration("mmap files");
    }

    std::unique_ptr<Tokenizer> tokenizer;
    std::vector<TokenInfo> old_code, new_code;
    try {
        tokenizer = GetTokenizer(tokenizer_mode, parser_mode, old_file, new_file, old_file_path,
                                 new_file_path);
        old_code = tokenizer->GetOldTokensInfo();
        new_code = tokenizer->GetNewTokensInfo();
    } catch (const std::exception& ex) {
        std::cerr << "Tokenizer Error: " << ex.what() << std::endl;
        return 2;
    }

    if (benchmark) {
        timer.Duration("tokenize");
    }

    Myers::Script script;
    try {
        script = Myers::ShortestEditScript(old_code, new_code);
    } catch (const std::exception& ex) {
        std::cerr << "Myers Algorithm Error: " << ex.what() << std::endl;
        return 3;
    }

    if (benchmark) {
        timer.Duration("diff");
    }

    try {
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
        diff_printer.Print(std::cout, script, tokenizer, old_code, new_code);
    } catch (const std::exception& ex) {
        std::cerr << "Print Diff Error: " << ex.what() << std::endl;
        return 4;
    }

    if (benchmark) {
        timer.Duration("print");
    }
    return 0;
}
