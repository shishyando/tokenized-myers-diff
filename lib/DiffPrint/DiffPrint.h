#pragma once

#include <ostream>
#include <vector>
#include <memory>

#include "lib/Tokenizer/Tokenizer.h"
#include "lib/MyersDiff/MyersDiff.h"

namespace DiffPrint {

enum Color { DEFAULT = 49, RED = 101, GREEN = 102 };

void Print(std::ostream& output, const Myers::Script& script,
           const std::unique_ptr<Tokenizer>& tokenizer, const std::vector<TokenInfo>& old_code,
           const std::vector<TokenInfo>& new_code);

}  // namespace DiffPrint
