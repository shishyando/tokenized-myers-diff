#include "../Tokenizer.h"
#include <iostream>

//TODO....
template <typename T>
void Test(const T& text, TokenizersTypes mode) {
    std::unique_ptr<Tokenizer<T>> a = GetTokenizer(mode, text);
    std::vector<TokenType> vec = a->GetTokens();
    for (auto i : vec) {
        std::cout << i << ' ';
    }
    std::cout << std::endl << "-------------" << std::endl;
}

int main() {
    const std::string test = "abacaba";
    const std::string test1 = "Never gonna give you up "
                              "Never gonna let you down "
                              "Never gonna run around and desert you "
                              "Never gonna make you cry "
                              "Never gonna say goodbye "
                              "Never gonna tell a lie and hurt you";
    const std::vector<char> test2 = {'a', 'b', 'a', 'c', 'a', 'b', 'a'};
    Test(test, TokenizersTypes::DEFAULT);
    Test(test1, TokenizersTypes::SPACE_SPLIT);
    Test(test2, TokenizersTypes::DEFAULT);
}