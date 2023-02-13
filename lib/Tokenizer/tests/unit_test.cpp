#include <iostream>
#include <sstream>

#include "../Tokenizer.h"

// TODO....
template <typename T>
void Test(const T& text, TokenizerMode tokenizer, ParserMode parser) {
    std::istringstream stream(text);
    std::unique_ptr<Tokenizer> a = GetTokenizer(tokenizer, parser);
    std::vector<CodeType> vec = a->GetTokenCodes(stream);
    for (auto i : vec) {
        std::cout << i << ' ';
    }
    std::cout << std::endl;
    for (auto i : vec) {
        std::cout << a->Decode(i) << "|";
    }
    std::cout << std::endl << "-------------" << std::endl;
}

int main() {
    const std::string test = "Эх Паша, Паша, Паша.";
    const std::string test1 =
        "Never gonna give you up\n"
        "Never gonna let you down\n"
        "Never gonna give you up\n"
        "Never gonna give you up\n"
        "Never gonna let you down\n"
        "Never gonna say goodbye\n"
        "Never gonna give you up\n"
        "Never gonna tell a lie and hurt you";
    const std::string test2 = "aba шшш";

    Test(test, TokenizerMode::WORD, ParserMode::UTF_8);
    Test(test1, TokenizerMode::LINE, ParserMode::BYTES);
    Test(test2, TokenizerMode::SYMBOL, ParserMode::UTF_8);
}
