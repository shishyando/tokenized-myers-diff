#pragma once

#include <ostream>
#include <vector>
#include <memory>

#include "lib/Tokenizer/Tokenizer.h"
#include "lib/MyersDiff/MyersDiff.h"

namespace DiffPrint {
    enum ColorBG {DEFAULT_BG = 49, RED_BG = 101, GREEN_BG = 102};
    enum ColorFG {BLACK_FG = 30, DEFAULT_FG = 39, BRIGHT_BLACK_FG = 90};
    enum TokenOwner {BOTH = 0, OLD = 1, NEW = 2};

class DiffPrinter {
public:
    struct Mode {
        bool raw = false;
        bool common_part = false;
        bool show_common_newlines = false;
        bool show_pos = false;
        std::string old_prefix;
        std::string new_prefix;
        std::string common_prefix;
    };

    Mode mode;

    void Print(std::ostream& output, const Myers::Script& script,
               const std::unique_ptr<Tokenizer>& tokenizer, const std::vector<TokenInfo>& old_code,
               const std::vector<TokenInfo>& new_code);

private:
    void PrintToken(std::ostream& output, TokenType token, TokenOwner owner);
    void PrintRawToken(std::ostream& output, TokenType token, std::string prefix);
    void PrintColoredToken(std::ostream& output, TokenType token, ColorFG color_fg, ColorBG color_bg);
    static void SetColors(std::ostream& output, ColorFG color_fg, ColorBG color_bg);
    static void ResetColors(std::ostream& output);

};

}  // namespace DiffPrint
