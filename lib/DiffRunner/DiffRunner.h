#pragma once

#include <filesystem>
#include <iostream>

#include "lib/Timer/Timer.h"
#include "lib/MyersDiff/MyersDiff.h"
#include "lib/Tokenizer/Tokenizer.h"
#include "lib/DiffPrint/DiffPrint.h"

namespace fs = std::filesystem;

class DiffRunner {
public:
    DiffRunner(TokenizerMode tokenizer_mode, ParserMode parser_mode,
               const DiffPrint::DiffPrinter& diff_printer, bool benchmark);

    void FileDiff(std::ostream& output, fs::path old_file_path, fs::path new_file_path);
    void DirDiff(std::ostream& output, fs::path old_dir_path, fs::path new_dir_path);

    long long GetMmapMs();
    long long GetTokenizeMs();
    long long GetDiffMs();
    long long GetPrintMs();

private:
    TokenizerMode tokenizer_mode_;
    ParserMode parser_mode_;
    DiffPrint::DiffPrinter diff_printer_;
    bool benchmark_;
    Timer timer_;

    long long mmap_miliseconds_;
    long long tokenize_miliseconds_;
    long long diff_miliseconds_;
    long long print_miliseconds_;

    const char* MmapFile(const char* file_path, size_t file_size);

    std::string FileTypeToString(fs::file_type type);
};
