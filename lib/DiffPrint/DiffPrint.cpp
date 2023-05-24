#include "DiffPrint.h"

#include <string>

namespace DiffPrint {

void DiffPrinter::Print(std::ostream& output, const Myers::Script& script,
                        const std::unique_ptr<Tokenizer>& tokenizer, const std::vector<TokenInfo>& old_code,
                        const std::vector<TokenInfo>& new_code) {
    size_t new_sz = new_code.size();
    size_t old_sz = old_code.size();
    size_t new_ind = 0, old_ind = 0;
    auto repl = script.begin();
    while (new_ind <= new_sz) {
        if (repl != script.end() && new_ind == repl->to_left) {
            old_ind = repl->from_left;
            if (mode.show_pos) {
                if (new_ind > 0) {
                    output << '\n';
                }
                output << old_code[old_ind].GetBegin() << ":" << new_code[new_ind].GetBegin() << '\n';
            }
            for (; old_ind < repl->from_right; ++old_ind) {
                DiffPrinter::PrintToken(output, tokenizer->DecodeOld(old_code[old_ind]), TokenOwner::OLD);
            }
            for (; new_ind < repl->to_right; ++new_ind) {
                DiffPrinter::PrintToken(output, tokenizer->DecodeNew(new_code[new_ind]), TokenOwner::NEW);
            }
            ++repl;
        } else {
            if (new_ind >= new_sz) {
                break;
            }
            DiffPrinter::PrintToken(output, tokenizer->DecodeNew(new_code[new_ind]), TokenOwner::BOTH);
            ++new_ind;
        }
    }
    output << '\n';
}

inline void DiffPrinter::PrintToken(std::ostream& output, TokenType token, TokenOwner owner) {
    if (mode.raw) {
        if (owner == TokenOwner::OLD) {
            PrintRawToken(output, token, mode.old_prefix);
        } else if (owner == TokenOwner::NEW) {
            PrintRawToken(output, token, mode.new_prefix);
        } else if (mode.common_part) {
            PrintRawToken(output, token, mode.common_prefix);
        }
    } else {
        if (owner == TokenOwner::OLD) {
            PrintColoredToken(output, token, ColorFG::BLACK_FG, ColorBG::RED_BG);
        } else if (owner == TokenOwner::NEW) {
            PrintColoredToken(output, token, ColorFG::BLACK_FG, ColorBG::GREEN_BG);
        } else if (mode.common_part) {
            PrintColoredToken(output, token, ColorFG::DEFAULT_FG, ColorBG::DEFAULT_BG);
        }
    }
}

inline void DiffPrinter::PrintRawToken(std::ostream& output, TokenType token, std::string prefix) {
    if (token.empty()) {
        return;
    }
    output << prefix << token;
}

inline void DiffPrinter::PrintColoredToken(std::ostream& output, TokenType token, ColorFG color_fg, ColorBG color_bg) {
    if (token.empty()) {
        return;
    }
    DiffPrinter::SetColors(output, color_fg, color_bg);
    if (token.back() != '\n') {
        output << token;
        DiffPrinter::ResetColors(output);
        return;
    }

    if (color_bg != ColorBG::DEFAULT_BG) {
        output << "\\n";
    } else if (mode.show_common_newlines) {
        DiffPrinter::SetColors(output, ColorFG::BRIGHT_BLACK_FG, ColorBG::DEFAULT_BG);
        output << "\033[2m\\n";
    }
    DiffPrinter::ResetColors(output);
    output << '\n';
}

inline void DiffPrinter::SetColors(std::ostream& output, ColorFG color_fg, ColorBG color_bg) {
    output << "\033[" + std::to_string(color_fg) + "m";
    output << "\033[" + std::to_string(color_bg) + "m";
}

inline void DiffPrinter::ResetColors(std::ostream& output) {
    output << "\033[0m";
}

}  // namespace DiffPrint
