#include "Tokenizer.h"
#include <stdexcept>
#include <cctype>

std::optional<TokenType> Tokenizer::GetSymbol(std::string_view& input, size_t pos) {
    if (pos >= input.size()) {
        return std::nullopt;
    }
    unsigned char byte = input[pos];
    if (parser_ == ParserMode::BYTES || !(byte >> 7)) {
        return input.substr(pos, 1);
    }
    unsigned char octets = 2;
    unsigned char mask = 1 << 5;
    while (byte & mask) {
        ++octets;
        mask >>= 1;
        if (octets > 4) {
            throw std::invalid_argument("not a utf-8 file: invalid byte sequence");
        }
    }
    size_t start = pos;
    while (--octets) {
        if (++pos >= input.size()) {
            throw std::invalid_argument("not a utf-8 file: unexpected EOF");
        }
        byte = input[pos];
        if ((byte & (0b11 << 6)) != (1 << 7)) {
            throw std::invalid_argument("not a utf-8 file: invalid byte sequence");
        }
    }
    return input.substr(start, pos - start + 1);
}

std::vector<CodeType> MapUsingTokenizers::GetTokenCodes(std::string_view& input) {
    std::vector<CodeType> ids;
    size_t pos = 0;
    while (std::optional<TokenType> token = GetToken(input, pos)) {
        size_t hash_val = std::hash<TokenType>()(token.value());
        while (hash2id_.contains(hash_val) && tokens_[hash2id_[hash_val]] != token.value()) {
            ++hash_val;
        }
        if (!hash2id_.contains(hash_val)) {
            hash2id_[hash_val] = last_code_++;
            tokens_.push_back(token.value());
        }
        ids.push_back(hash2id_[hash_val]);
        pos += token.value().size();
    }
    return ids;
}

TokenType MapUsingTokenizers::Decode(CodeType code) {
    return tokens_[code];
}

std::optional<TokenType> SymbolTokenizer::GetToken(std::string_view& input, size_t pos) {
    return GetSymbol(input, pos);
}

template <typename IsSplitter>
std::optional<TokenType> SymbolSplitTokenizer<IsSplitter>::GetToken(std::string_view& input,
                                                                    size_t pos) {
    TokenType token;
    if (!last_read_splitter_.empty()) {
        token = last_read_splitter_;
        last_read_splitter_ = {};
        return token;
    }
    size_t start = pos;
    while (std::optional<TokenType> symbol = GetSymbol(input, pos)) {
        if (IsSplitter{}(symbol.value())) {
            last_read_splitter_ = symbol.value();
            break;
        }
        pos += symbol.value().size();
    }
    size_t len = pos - start;
    if (len == 0) {
        if (last_read_splitter_.empty()) {
            return std::nullopt;
        }
        token = last_read_splitter_;
        last_read_splitter_ = {};
        return token;
    }
    return input.substr(start, len);
}

std::unique_ptr<Tokenizer> GetTokenizer(TokenizerMode tokenizer, ParserMode parser) {
    struct IsWordSplitter {
        bool operator()(TokenType& token) {
            return token.size() == 1 && isspace(token[0]);
        }
    };
    struct IsLineSplitter {
        bool operator()(TokenType& token) {
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
