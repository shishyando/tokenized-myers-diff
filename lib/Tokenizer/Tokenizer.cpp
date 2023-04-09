#include "Tokenizer.h"
#include <stdexcept>
#include <cctype>

std::optional<TokenType> Tokenizer::GetSymbol(std::string_view& input, size_t pos) {
    if (pos >= input.size()) {
        return std::nullopt;
    }
    int byte = input[pos];
    if (byte == std::char_traits<char>::eof()) {
        return std::nullopt;
    }
    if (parser_ == ParserMode::BYTES || !(byte >> 7)) {
        return TokenType{static_cast<char>(byte)};
    }
    TokenType symbol;
    symbol.push_back(byte);
    char octets = 2;
    char mask = 1 << 5; 
    while (byte & mask) {
        ++octets;
        mask >>= 1;
        if (octets > 4) {
            throw std::invalid_argument("not a utf-8 file: invalid byte sequence");
        }
    }
    while (--octets) {
        if (++pos >= input.size()) {
            throw std::invalid_argument("not a utf-8 file: unexpected EOF");
        }
        byte = input[pos];
        if (byte == std::char_traits<char>::eof()) {
            throw std::invalid_argument("not a utf-8 file: unexpected EOF");
        }
        if ((byte & (0b11 << 6)) != (1 << 7)) {
            throw std::invalid_argument("not a utf-8 file: invalid byte sequence");
        }
        symbol.push_back(byte);
    }
    return symbol;
}

std::vector<CodeType> MapUsingTokenizers::GetTokenCodes(std::string_view& input) {
    std::vector<CodeType> tokenized;
    size_t pos = 0;
    while (std::optional<TokenType> token = GetToken(input, pos)) {
        if (!code_by_token_.contains(token.value())) {
            CodeType code = code_by_token_.size();
            code_by_token_[token.value()] = code;
            token_by_code_[code] = token.value();
        }
        tokenized.push_back(code_by_token_[token.value()]);
        pos += token.value().size();
    }
    return tokenized;
}

TokenType MapUsingTokenizers::Decode(CodeType code) {
    return token_by_code_[code];
}

std::optional<TokenType> SymbolTokenizer::GetToken(std::string_view& input, size_t pos) {
    return GetSymbol(input, pos);
}

template<typename IsSplitter>
std::optional<TokenType> SymbolSplitTokenizer<IsSplitter>::GetToken(std::string_view& input, size_t pos) {
    TokenType token;
    if (!last_read_splitter_.empty()) {
        token = last_read_splitter_;
        last_read_splitter_.clear();
        return token;
    }
    while (std::optional<TokenType> symbol = GetSymbol(input, pos)) {
        if (IsSplitter{}(symbol.value())) {
            last_read_splitter_ = symbol.value();
            break;
        }
        token += symbol.value();
        pos += symbol.value().size();
    }
    if (token.empty()) {
        if (last_read_splitter_.empty()) {
            return std::nullopt;
        }
        token = last_read_splitter_;
        last_read_splitter_.clear();
    }
    return token;
}

std::unique_ptr<Tokenizer> GetTokenizer(TokenizerMode tokenizer, ParserMode parser) {
    struct IsWordSplitter {
        bool operator() (TokenType& token) {
            return token.size() == 1 && isspace(token[0]);
        }
    };
    struct IsLineSplitter {
        bool operator() (TokenType& token) {
            return token == "\n";
        }
    };

    if (tokenizer == TokenizerMode::SYMBOL) {
        return std::make_unique<SymbolTokenizer>(parser);
    } else if (tokenizer == TokenizerMode::WORD) {
        return std::make_unique<SymbolSplitTokenizer<IsWordSplitter>>(parser);
    } else if (tokenizer == TokenizerMode::LINE) {
        return std::make_unique<SymbolSplitTokenizer<IsLineSplitter>>(parser);
    } else {
        throw std::invalid_argument("no such tokenizer type");
    }
}
