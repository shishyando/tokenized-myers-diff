#include <iostream>
#include <sstream>

#include "../Tokenizer.h"

// TODO....
void Test(std::string_view text, TokenizerMode tokenizer_mode, ParserMode parser) {
    std::unique_ptr<Tokenizer> tokenizer = GetTokenizer(tokenizer_mode, parser, "", text);
    std::vector<TokenInfo> vec = tokenizer->GetNewTokensInfo();
    for (auto i : vec) {
        std::cout << i.GetId() << ' ';
    }
    std::cout << std::endl;
    for (auto i : vec) {
        std::cout << tokenizer->DecodeNew(i) << "|";
    }
    std::cout << std::endl << "-------------" << std::endl;
}

int main() {
    const std::string test = "Эх Паша, Паша,   Паша.   ";
    const std::string test1 =
        "Never gonna give you up\n"
        "Never gonna let you down\n"
        "Never gonna give you up\n"
        "Never gonna give you up\n"
        "Never gonna let you down\n"
        "Never gonna say goodbye\n"
        "Never gonna give you up\n"
        "Never gonna tell a lie and hurt you";
    const std::string test2 = "aba   шшш";

    Test(test, TokenizerMode::WORD, ParserMode::UTF_8);
    Test(test1, TokenizerMode::LINE, ParserMode::BYTES);
    Test(test2, TokenizerMode::SYMBOL, ParserMode::UTF_8);
}
