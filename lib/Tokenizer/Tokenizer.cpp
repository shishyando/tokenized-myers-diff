#include "Tokenizer.h"

#include <stdexcept>
#include <cctype>

std::optional<TokenType> Tokenizer::GetSymbol(std::istream& input) {
    int byte = input.get();
    if (byte == std::char_traits<char>::eof()) {
        return std::nullopt;
    }
    if (parser_ == ParserMode::BYTES || !(byte >> 7)) {
        return TokenType{static_cast<char>(byte)};
    }
    TokenType symbol;
    symbol.push_back(byte);
    char octets = 2;
    while (!((byte << octets) >> 7)) {
        ++octets;
        if (octets > 4) {
            throw std::invalid_argument("not a utf-8 file");
        }
    }
    while (--octets) {
        byte = input.get();
        if (byte == std::char_traits<char>::eof() || (byte & (0b11 << 6)) != (1 << 7)) {
            throw std::invalid_argument("not a utf-8 file");
        }
        symbol.push_back(byte);
    }
    return symbol;
}

std::vector<CodeType> MapUsingTokenizers::GetTokenCodes(std::istream& input) {
    std::vector<CodeType> tokenized;
    while (std::optional<TokenType> token = GetToken(input)) {
        if (code_by_token_.contains(token.value())) {
            CodeType code = code_by_token_.size();
            code_by_token_[token.value()] = code;
            token_by_code_[code] = token.value();
        }
        tokenized.push_back(code_by_token_[token.value()]);
    }
    return tokenized;
}

TokenType MapUsingTokenizers::Decode(CodeType code) {
    return token_by_code_[code];
}

std::optional<TokenType> SymbolTokenizer::GetToken(std::istream& input) {
    return GetSymbol(input);
}

std::optional<TokenType> WordTokenizer::GetToken(std::istream& input) {
    TokenType token;
    while (std::optional<TokenType> symbol = GetSymbol(input)) {
        token += symbol.value();
        if (symbol.value().size() == 1 && std::isspace(symbol.value()[0])) {
            break;
        }
    }
    return token;
}

std::optional<TokenType> LineTokenizer::GetToken(std::istream& input) {
    TokenType token;
    while (std::optional<TokenType> symbol = GetSymbol(input)) {
        token += symbol.value();
        if (symbol.value() != "\n") {
            break;
        }
    }
    return token;
}

std::unique_ptr<Tokenizer> GetTokenizer(TokenizerMode tokenizer, ParserMode parser) {
    if (tokenizer == TokenizerMode::SYMBOL) {
        return std::make_unique<SymbolTokenizer>(parser);
    } else if (tokenizer == TokenizerMode::WORD) {
        return std::make_unique<WordTokenizer>(parser);
    } else if (tokenizer == TokenizerMode::LINE) {
        return std::make_unique<LineTokenizer>(parser);
    } else {
        throw std::invalid_argument("not such tokenizer type");
    }
}

