#include <iostream>
#include <fstream>
#include <map>
#include <string>

#include "lib/ArgParser/ArgParser.h"
#include "lib/DiffPrint/DiffPrint.h"
#include "lib/MyersDiff/MyersDiff.h"
#include "lib/Tokenizer/Tokenizer.h"

const std::string ARGS_FORMAT = "Usage: diff old new [tokenizer parser]\n"
                                "   old=<path_to_file1>\n"
                                "   new=<path_to_file2>\n"
                                "   tokenizer=<tokenizer_mode>   possible modes: symbol, word, line   default: word\n"
                                "   parser=<parser_mode>         possible modes: bytes, utf-8         default: utf-8\n";

int main(int argc, char *argv[]) {
    ArgParser argParser(argc, argv);
    argParser.Add("old", "old_file");
    argParser.Add("new", "new_file");
    argParser.Add("tokenizer", "tokenizer");
    argParser.Add("parser", "parser_mode");
    try {
        argParser.Build();
        if (!argParser.Get("old_file").has_value() || !argParser.Get("new_file").has_value()) {
            throw std::invalid_argument("no input files given");
        }
    } catch (...) {
        std::cout << ARGS_FORMAT << std::endl;
        return 0;
    }
    std::string old_file_path = argParser.Get("old_file").value();
    std::string new_file_path = argParser.Get("new_file").value();
    std::string tokeninzer_arg = (argParser.Get("tokenizer").has_value() ? argParser.Get("tokenizer").value() : "word");
    std::string parser_arg = (argParser.Get("parser_mode").has_value() ? argParser.Get("parser_mode").value() : "utf-8");

    std::cerr << "old file path: " << old_file_path << '\n';
    std::cerr << "new file path: " << new_file_path << '\n';
    std::cerr << "old file path: " << tokeninzer_arg << '\n';
    std::cerr << "old file path: " << parser_arg << '\n';
    TokenizerMode tokenizer_mode;
    if (tokeninzer_arg == "symbol") {
        tokenizer_mode = TokenizerMode::SYMBOL;
    } else if (tokeninzer_arg == "word") {
        tokenizer_mode = TokenizerMode::WORD;
    } else if (tokeninzer_arg == "line") {
        tokenizer_mode = TokenizerMode::LINE;
    } else {
        std::cerr << ARGS_FORMAT << std::endl;
        return 0;
    }
    ParserMode parser_mode;
    if (parser_arg == "bytes") {
        parser_mode = ParserMode::BYTES;
    } else if (parser_arg == "utf-8") {
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
        tokenizer = GetTokenizer(tokenizer_mode, parser_mode);
        old_code = tokenizer->GetTokenCodes(old_file);
        new_code = tokenizer->GetTokenCodes(new_file);
    } catch (const std::exception & ex) {
        std::cerr << "Tokenizer Error: " << ex.what() << std::endl;
        return 1;
    }

    Myers::Script script;
    try {
        script = Myers::ShortestEditScript<CodeType>(old_code, new_code);
    } catch (const std::exception & ex) {
        std::cerr << "Myers Algorithm Error: " << ex.what() << std::endl;
        return 2;
    }

    try {
        DiffPrint::Print(std::cout, script, tokenizer, old_code, new_code);
    } catch (const std::exception & ex) {
        std::cerr << "Print Diff Error: " << ex.what() << std::endl;
        return 3;
    }

    return 0;
}
