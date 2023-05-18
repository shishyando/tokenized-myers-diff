#include "DiffPrint.h"

#include <string>

namespace DiffPrint {

static void PrintColoredToken(std::ostream& output, TokenType token, Color color) {
    if (!token.empty() && token[token.size() - 1] == '\n' && color != Color::DEFAULT) {
        output << "\033[" + std::to_string(color) + "m";
        output << token.substr(0, token.size() - 1);
        output << "\\n";
        output << "\033[0m";
        output << '\n';
        return;
    }
    output << "\033[" + std::to_string(color) + "m";
    output << token;
    output << "\033[0m";
}

void Print(std::ostream& output, const Myers::Script& script,
           const std::unique_ptr<Tokenizer>& tokenizer, const std::vector<TokenInfo>& old_code,
           const std::vector<TokenInfo>& new_code) {
    size_t old_sz = old_code.size();
    size_t old_ind = 0;
    auto repl = script.begin();
    while (old_ind <= old_sz) {
        if (repl != script.end() && old_ind == repl->from_left) {
            for (; old_ind < repl->from_right; ++old_ind) {
                PrintColoredToken(output, tokenizer->DecodeOld(old_code[old_ind]), Color::RED);
            }
            for (size_t new_ind = repl->to_left; new_ind < repl->to_right; ++new_ind) {
                PrintColoredToken(output, tokenizer->DecodeNew(new_code[new_ind]),
                                  Color::GREEN);
            }
            ++repl;
        } else {
            if (old_ind >= old_sz) {
                break;
            }
            PrintColoredToken(output, tokenizer->DecodeOld(old_code[old_ind]), Color::DEFAULT);
            ++old_ind;
        }
    }
    output << '\n';
}

}  // namespace DiffPrint
