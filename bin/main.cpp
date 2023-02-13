#include <iostream>
#include <fstream>
#include <map>
#include <string>

#include "lib/ArgParser/ArgParser.h"
#include "lib/DiffPrint/DiffPrint.h"
#include "lib/MyersDiff/MyersDiff.h"
#include "lib/Tokenizer/Tokenizer.h"

const std::string ARGS_FORMAT = "Please use: diff old=<path_to_file1> new=<path_to_file2> [token=<tokenizer_mode>] [parser=<parser_mode>]";

int main(int argc, char *argv[]) {
    ArgParser argParser(argc, argv);
    argParser.Add("old", "old_file");
    argParser.Add("new", "new_file");
    argParser.Add("token", "token_mode");
    argParser.Add("parser", "parser_mode");
    try {
        argParser.Build();
    } catch (...) {
        std::cout << ARGS_FORMAT << std::endl;
        return 0;
    }
    std::string old_file_path = (argParser.Get("old_file").has_value() ? argParser.Get("old_file").value() : "");
    std::string new_file_path = (argParser.Get("new_file").has_value() ? argParser.Get("new_file").value() : "");
    std::string token_str = (argParser.Get("token_mode").has_value() ? argParser.Get("token_mode").value() : "word");
    std::string parser_str = (argParser.Get("parser_mode").has_value() ? argParser.Get("parser_mode").value() : "utf-8");

    TokenizerMode token_mode;
    if (token_str == "symbol") {
        token_mode = TokenizerMode::SYMBOL;
    } else if (token_str == "word") {
        token_mode = TokenizerMode::WORD;
    } else if (token_str == "line") {
        token_mode = TokenizerMode::LINE;
    } else {
        std::cout << ARGS_FORMAT << std::endl;
        return 0;
    }
    ParserMode parser_mode;
    if (parser_str == "bytes") {
        parser_mode = ParserMode::BYTES;
    } else if (parser_str == "utf-8") {
        parser_mode = ParserMode::UTF_8;
    } else {
        std::cout << ARGS_FORMAT << std::endl;
        return 0;
    }
    std::ifstream old_file, new_file;
    try {
        old_file.open(old_file_path);
        new_file.open(new_file_path);
    } catch (...) {
        std::cout << ARGS_FORMAT << std::endl;
        return 0;
    }

    std::unique_ptr<Tokenizer> tokenizer;
    std::vector<CodeType> old_code, new_code;
    try {
        tokenizer = GetTokenizer(token_mode, parser_mode);
        old_code = tokenizer->GetTokenCodes(old_file);
        new_code = tokenizer->GetTokenCodes(new_file);
    } catch (...) {
        std::cout << "Tokenizer Error" << std::endl;
        return 1;
    }

    Myers::Script script;
    try {
        script = Myers::ShortestEditScript<CodeType>(old_code, new_code);
    } catch (...) {
        std::cout << "Myers Algorithm Error" << std::endl;
        return 2;
    }

    try {
        DiffPrint::Print(std::cout, script, tokenizer, old_code, new_code);
    } catch (...) {
        std::cout << "Print Diff Error" << std::endl;
        return 3;
    }

    return 0;
}
