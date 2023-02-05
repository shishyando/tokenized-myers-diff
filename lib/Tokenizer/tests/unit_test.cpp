#include "../Tokenizer.h"
#include <iostream>

//TODO....
int main() {
    const std::string test = "abacaba";
    std::unique_ptr<Tokenizer> a = GetTokenizer(TokenizersTypes::DEFAULT, test);
    std::vector<TokenType> vec = a->GetTokens();
    for (auto i : vec) {
        std::cout << i << ' ';
    }
    std::cout << std::endl << "-------------" << std::endl;
    const std::string test1 = "Never gonna give you up "
                  "Never gonna let you down "
                  "Never gonna run around and desert you "
                  "Never gonna make you cry "
                  "Never gonna say goodbye "
                  "Never gonna tell a lie and hurt you";
    std::unique_ptr<Tokenizer> b = GetTokenizer(TokenizersTypes::SPACE_SPLIT, test1);
    std::vector<TokenType> vec1 = b->GetTokens();
    for (auto i : vec1) {
        std::cout << i << ' ';
    }
    std::cout << std::endl;
}