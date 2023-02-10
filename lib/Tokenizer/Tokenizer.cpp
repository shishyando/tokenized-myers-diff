#include <unordered_map>
#include <stdexcept>
#include "Tokenizer.h"
#include "IterableObject/IterableObject.h"

template <typename TextType>
std::vector<TokenType> SplitTokenizer<TextType>::GetTokens() {
    std::vector<TokenType> tokenized;
    std::unordered_map<IterableObject<TextType>,
            TokenType,
            IterableObjectHasher<TextType>> map;
    size_t begin = 0;
    size_t end = GetNextTokenPos(0);
    while (begin != _text.size()) {
        IterableObject<TextType> token(_text, begin, end);
        if (map.count(token) == 0) {
            map[token] = static_cast<int>(map.size());
        }
        tokenized.push_back(map[token]);
        begin = end;
        end = GetNextTokenPos(end);
    }
    return tokenized;
}

template <typename TextType>
size_t SplitTokenizer<TextType>::GetNextTokenPos(size_t pos) {
    if (pos == _text.size()) {
        return pos;
    }
    size_t ret = pos;
    while (ret < _text.size() && IsSameType(static_cast<TokenType>(_text[pos]),
                                            static_cast<TokenType>(_text[ret]))) {
        ++ret;
    }
    return ret;
}

template <typename TextType>
bool SplitTokenizer<TextType>::IsSameType(TokenType a, TokenType b) {
    if (a == _splitter) {
        return b == _splitter;
    }
    return b != _splitter;
}

template <typename TextType>
std::vector<TokenType> SymbolTokenizer<TextType>::GetTokens() {
    std::vector<TokenType> tokenized;
    for (char i : _text) {
        tokenized.push_back(static_cast<TokenType>(i));
    }
    return tokenized;
}

template <typename TextType>
std::unique_ptr<Tokenizer<TextType>> GetTokenizer(TokenizersTypes type, const TextType& text) {
    if (type == TokenizersTypes::DEFAULT) {
        return std::make_unique<SymbolTokenizer<TextType>>(text);
    } else if (type == TokenizersTypes::SPACE_SPLIT) {
        return std::make_unique<SplitTokenizer<TextType>>(text, ' ');
    } else {
        throw std::invalid_argument("Not such tokenizer type");
    }
}