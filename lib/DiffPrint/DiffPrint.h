#pragma once

#include <ostream>
#include <string>
#include <vector>

namespace DiffPrint {

enum Color {
    DEFAULT = 0,
    RED = 31,
    GREEN = 32
};

template<typename TToken>
static void PrintColoredToken(std::ostream& output, TToken token, Color color) {
    output << "\033[" + std::to_string(color) + "m";
    output << token;
    output << "\033[0m";
}

template<typename TScript, typename TTokenizer, typename TCode>
void Print(std::ostream& output, const TScript& script, const std::unique_ptr<TTokenizer>& tokenizer, const std::vector<TCode>& old_code, const std::vector<TCode>& new_code) {
    size_t old_sz = old_code.size();
    size_t old_ind = 0;
    auto repl = script.begin();
    while (old_ind <= old_sz) {
        if (repl != script.end() && old_ind == repl->from_left) {
            for (; old_ind < repl->from_right; ++old_ind) {
                PrintColoredToken(output, tokenizer->Decode(old_code[old_ind]), Color::RED);
            }
            for (size_t new_ind = repl->to_left; new_ind < repl->to_right; ++new_ind) {
                PrintColoredToken(output, tokenizer->Decode(new_code[new_ind]), Color::GREEN);
            }
            ++repl;
        } else {
            if (old_ind >= old_sz) {
                break;
            }
            PrintColoredToken(output, tokenizer->Decode(old_code[old_ind]), Color::DEFAULT);
            ++old_ind;
        }
    }
}

} // namespace DiffPrint
