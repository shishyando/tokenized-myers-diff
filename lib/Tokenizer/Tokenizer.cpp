#include "Tokenizer.h"

#include <stdexcept>

bool Tokenizer::_GetNextSymbol(TokenType& c) {
    c.clear();
    int byte = _stream.get();
    if (byte == -1) {
        return false;
    }
    c += std::string(1, static_cast<char>(byte));
    if (!_is_utf_8 || !(byte >> 7)) {
        return true;
    }
    char octets = 2;
    while (!((byte << octets) >> 7)) {
        ++octets;
        if (octets > 4) {
            throw std::invalid_argument("not a utf-8 file");
        }
    }
    while (--octets) {
        byte = _stream.get();
        if (byte == -1 || (byte & ((1 << 7) | (1 << 6))) != (1 << 7)) {
            throw std::invalid_argument("not a utf-8 file");
        }
        c += std::string(1, static_cast<char>(byte));
    }
    return true;
}

std::vector<CodeType> MapUsingTokenizers::GetTokensCodes() {
    std::vector<CodeType> tokenized;
    std::unordered_map<TokenType, CodeType> map;
    TokenType token;
    while (_GetNextToken(token)) {
        if (map.count(token) == 0) {
            CodeType code = static_cast<CodeType>(map.size());
            map[token] = code;
            _code_to_token[code] = token;
        }
        tokenized.push_back(map[token]);
    }
    return tokenized;
}

TokenType MapUsingTokenizers::Decode(CodeType code) {
    return _code_to_token[code];
}

bool SplitTokenizer::_GetNextToken(TokenType& token) {
    static bool read_unhandled_splitter = false;
    token.clear();
    if (read_unhandled_splitter) {
        read_unhandled_splitter = false;
        token += _splitter;
        return true;
    }
    TokenType c;
    while (_GetNextSymbol(c)) {
        if (c == _splitter) {
            if (token.size() == 0) {
                token += c;
            } else {
                read_unhandled_splitter = true;
            }
            return true;
        }
        token += c;
    }
    return token.size() != 0;
}

bool SymbolTokenizer::_GetNextToken(TokenType& token) {
    return _GetNextSymbol(token);
}

std::unique_ptr<Tokenizer> GetTokenizer(TokenizersType tokenizer,
                                        ParserType parser,
                                        std::istream& stream) {
    if (tokenizer == TokenizersType::DEFAULT) {
        return std::make_unique<SymbolTokenizer>(stream, parser);
    } else if (tokenizer == TokenizersType::SPACE_SPLIT) {
        return std::make_unique<SplitTokenizer>(" ", stream, parser);
    } else {
        throw std::invalid_argument("not such tokenizer type");
    }
}
