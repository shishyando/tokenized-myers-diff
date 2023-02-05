#include "Tokenizer.h"
#include <unordered_map>
#include "IterableObject/IterableObject.h"

std::vector<TokenType> SplitTokenizer::GetTokens() {
    std::vector<TokenType> tokenized;
    std::unordered_map<IterableObject,
            TokenType,
            IterableObjectHasher> map;
    size_t begin = 0;
    size_t end = GetNextTokenPos(0);
    while (begin != _text.size()) {
        IterableObject token(_text, begin, end);
        if (map.count(token) == 0) {
            map[token] = static_cast<int>(map.size());
        }
        tokenized.push_back(map[token]);
        begin = end;
        end = GetNextTokenPos(end);
    }
    return tokenized;
}


std::size_t SplitTokenizer::GetNextTokenPos(size_t pos) {
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

bool SplitTokenizer::IsSameType(TokenType a, TokenType b) {
    if (a == _splitter) {
        return b == _splitter;
    }
    return b != _splitter;
}

std::vector<TokenType> SymbolTokenizer::GetTokens() {
    std::vector<TokenType> tokenized;
    for (char i : _text) {
        tokenized.push_back(static_cast<TokenType>(i));
    }
    return tokenized;
}

std::unique_ptr<Tokenizer> GetTokenizer(TokenizersTypes type, const TextType& text) {
    if (type == TokenizersTypes::DEFAULT) {
        return std::make_unique<SymbolTokenizer>(text);
    } else if (type == TokenizersTypes::SPACE_SPLIT) {
        return std::make_unique<SplitTokenizer>(text, ' ');
    } else {
        throw std::invalid_argument("Not such tokenizer type");
    }
}