#include "Tokenizer.h"

#include <stdexcept>
#include <cctype>

TokenInfo::TokenInfo(TokenId id, uint32_t begin_pos) : id_(id), begin_pos_(begin_pos) {
}

bool TokenInfo::operator==(const TokenInfo& rhs) const {
    return id_ == rhs.id_;
}

TokenId TokenInfo::GetId() const {
    return id_;
}

uint32_t TokenInfo::GetBegin() const {
    return begin_pos_;
}

Tokenizer::Tokenizer(ParserMode parser, std::string_view old_data, std::string_view new_data)
    : parser_(parser), old_data_(old_data), new_data_(new_data) {
}

std::optional<TokenType> Tokenizer::GetSymbol(std::string_view input, size_t pos) {
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

std::vector<TokenInfo> Tokenizer::GetOldTokensInfo() {
    return GetTokenInfos(old_data_);
}

std::vector<TokenInfo> Tokenizer::GetNewTokensInfo() {
    return GetTokenInfos(new_data_);
}

TokenType Tokenizer::DecodeOld(const TokenInfo& code) {
    return GetToken(old_data_, code.GetBegin()).value();
}

TokenType Tokenizer::DecodeNew(const TokenInfo& code) {
    return GetToken(new_data_, code.GetBegin()).value();
}

MapUsingTokenizers::MapUsingTokenizers(ParserMode parser, std::string_view old_data,
                                       std::string_view new_data)
    : Tokenizer(parser, old_data, new_data) {
}

std::vector<TokenInfo> MapUsingTokenizers::GetTokenInfos(std::string_view input) {
    std::vector<TokenInfo> ids;
    size_t pos = 0;
    while (std::optional<TokenType> token = GetToken(input, pos)) {
        size_t hash_val = GetHash(token.value());
        while (hash2id_.contains(hash_val) &&
               !IsEqual(tokens_[hash2id_[hash_val]], token.value())) {
            ++hash_val;
        }
        if (!hash2id_.contains(hash_val)) {
            hash2id_[hash_val] = last_code_++;
            tokens_.push_back(token.value());
        }
        ids.emplace_back(hash2id_[hash_val], pos);
        pos += token.value().size();
    }
    return ids;
}

bool MapUsingTokenizers::IsEqual(TokenType lhs, TokenType rhs) {
    return lhs == rhs;
}

size_t MapUsingTokenizers::GetHash(TokenType token) {
    return std::hash<TokenType>{}(token);
}

SymbolTokenizer::SymbolTokenizer(ParserMode parser, std::string_view old_data,
                                 std::string_view new_data)
    : MapUsingTokenizers(parser, old_data, new_data) {
}

std::optional<TokenType> SymbolTokenizer::GetToken(std::string_view input, size_t pos) {
    return GetSymbol(input, pos);
}

template <typename IsSplitter>
SymbolSplitTokenizer<IsSplitter>::SymbolSplitTokenizer(ParserMode parser, std::string_view old_data,
                                                       std::string_view new_data)
    : MapUsingTokenizers(parser, old_data, new_data) {
}

template <typename IsSplitter>
std::optional<TokenType> SymbolSplitTokenizer<IsSplitter>::GetToken(std::string_view input,
                                                                    size_t pos) {
    if (pos >= input.size()) {
        return std::nullopt;
    }
    size_t start = pos;
    while (std::optional<TokenType> symbol = GetSymbol(input, pos)) {
        if (IsSplitter{}(symbol.value())) {
            break;
        }
        pos += symbol.value().size();
    }
    if (pos == start) {
        return input.substr(start, 1);
    }
    return input.substr(start, pos - start);
}

bool IsWordSplitter::operator()(TokenType token) const {
    return token.size() == 1 && isspace(token[0]);
}

bool IsLineSplitter::operator()(TokenType token) const {
    return token == "\n";
}

IgnoreAllSpaceTokenizer::IgnoreAllSpaceTokenizer(ParserMode parser, std::string_view old_data,
                                                 std::string_view new_data)
    : LineTokenizer(parser, old_data, new_data) {
}

bool IgnoreAllSpaceTokenizer::IsEqual(TokenType lhs, TokenType rhs) {
    if (lhs == "\n" || rhs == "\n") {
        return lhs == rhs;
    }
    auto get_next_nonspace = [](size_t pos, const TokenType& token) {
        while (pos < token.size()) {
            if (token[pos] != ' ' && token[pos] != '\t') {
                break;
            }
            ++pos;
        }
        return pos;
    };
    size_t lhs_pos = get_next_nonspace(0, lhs);
    size_t rhs_pos = get_next_nonspace(0, rhs);
    while (lhs_pos < lhs.size()) {
        if (rhs_pos >= rhs.size() || rhs[rhs_pos] != lhs[lhs_pos]) {
            return false;
        }
        lhs_pos = get_next_nonspace(lhs_pos + 1, lhs);
        rhs_pos = get_next_nonspace(rhs_pos + 1, rhs);
    }
    return rhs_pos == rhs.size();
}

size_t IgnoreAllSpaceTokenizer::GetHash(TokenType token) {
    if (token == "\n") {
        return 0;
    }
    std::string non_space_token;
    non_space_token.reserve(token.size());
    size_t pos = 0;
    while (pos < token.size()) {
        if (token[pos] != ' ' && token[pos] != '\t') {
            non_space_token.push_back(token[pos]);
        }
        ++pos;
    }
    return std::hash<std::string>{}(non_space_token);
}

IgnoreSpaceChangeTokenizer::IgnoreSpaceChangeTokenizer(ParserMode parser, std::string_view old_data,
                                                       std::string_view new_data)
    : LineTokenizer(parser, old_data, new_data) {
}

bool IgnoreSpaceChangeTokenizer::IsEqual(TokenType rhs, TokenType lhs) {
    if (lhs == "\n" || rhs == "\n") {
        return lhs == rhs;
    }

    auto is_space = [](char c) { return c == ' ' || c == '\t'; };

    auto get_next_nonspace = [&is_space](size_t pos, const TokenType& token) {
        while (pos < token.size()) {
            if (!is_space(token[pos])) {
                break;
            }
            ++pos;
        }
        return pos;
    };
    size_t lhs_pos = 0;
    size_t rhs_pos = 0;
    while (lhs_pos < lhs.size()) {
        if (rhs_pos >= rhs.size()) {
            return false;
        }
        if (is_space(lhs[lhs_pos])) {
            if (!is_space(rhs[rhs_pos])) {
                return false;
            }
            lhs_pos = get_next_nonspace(lhs_pos, lhs);
            rhs_pos = get_next_nonspace(rhs_pos, rhs);
        } else if (is_space(rhs[rhs_pos]) || rhs[rhs_pos] != lhs[lhs_pos]) {
            return false;
        } else {
            ++lhs_pos;
            ++rhs_pos;
        }
    }
    return rhs_pos == rhs.size();
}

size_t IgnoreSpaceChangeTokenizer::GetHash(TokenType token) {
    if (token == "\n") {
        return 0;
    }
    std::string space_num_ignore_token;
    space_num_ignore_token.reserve(token.size());
    size_t pos = 0;
    while (pos < token.size()) {
        if (token[pos] != ' ' && token[pos] != '\t') {
            space_num_ignore_token.push_back(token[pos]);
        } else if (space_num_ignore_token.empty() || space_num_ignore_token.back() != ' ') {
            space_num_ignore_token.push_back(' ');
        }
        ++pos;
    }
    return std::hash<std::string>{}(space_num_ignore_token);
}

std::unique_ptr<Tokenizer> GetTokenizer(TokenizerMode tokenizer, ParserMode parser,
                                        std::string_view old_data, std::string_view new_data) {
    if (tokenizer == TokenizerMode::SYMBOL) {
        return std::make_unique<SymbolTokenizer>(parser, old_data, new_data);
    } else if (tokenizer == TokenizerMode::WORD) {
        return std::make_unique<WordTokenizer>(parser, old_data, new_data);
    } else if (tokenizer == TokenizerMode::LINE) {
        return std::make_unique<LineTokenizer>(parser, old_data, new_data);
    } else if (tokenizer == TokenizerMode::IGNORE_ALL_SPACE) {
        return std::make_unique<IgnoreAllSpaceTokenizer>(parser, old_data, new_data);
    } else if (tokenizer == TokenizerMode::IGNORE_SPACE_CHANGE) {
        return std::make_unique<IgnoreSpaceChangeTokenizer>(parser, old_data, new_data);
    } else {
        throw std::invalid_argument("no such tokenizer type");
    }
}
