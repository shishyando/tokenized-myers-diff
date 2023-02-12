#include <iostream>
#include <sstream>

#include "../Tokenizer.h"

// TODO....
template <typename T>
void Test(const T& text, TokenizersType tokenizer, ParserType parser) {
    std::istringstream stream(text);
    std::unique_ptr<Tokenizer> a = GetTokenizer(tokenizer, parser, stream);
    std::vector<CodeType> vec = a->GetTokensCodes();
    for (auto i : vec) {
        std::cout << i << ' ';
    }
    for (auto i : vec) {
        std::cout << a->Decode(i) << "|";
    }
    std::cout << std::endl << "-------------" << std::endl;
}

int main() {
    const std::string test = "Эх Паша, Паша, Паша.";
    const std::string test1 =
        "Never gonna give you up "
        "Never gonna let you down "
        "Never gonna run around and desert you "
        "Never gonna make you cry "
        "Never gonna say goodbye "
        "Never gonna tell a lie and hurt you";
    const std::string test2 = "aba шшш";
    Test(test, TokenizersType::SPACE_SPLIT, ParserType::BY_UTF_8);
    Test(test1, TokenizersType::SPACE_SPLIT, ParserType::BY_CHARS);
    Test(test2, TokenizersType::DEFAULT, ParserType::BY_UTF_8);
}
