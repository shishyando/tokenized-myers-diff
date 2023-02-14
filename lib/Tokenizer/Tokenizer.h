#pragma once

#include <istream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <optional>

using CodeType = size_t;
using TokenType = std::string;

enum class ParserMode { BYTES, UTF_8 };

enum class TokenizerMode { SYMBOL, WORD, LINE };

class Tokenizer {
public:
    explicit Tokenizer(ParserMode parser) : parser_(parser) {
    }

    virtual std::vector<CodeType> GetTokenCodes(std::istream& in) = 0;
    virtual TokenType Decode(CodeType code) = 0;
    virtual ~Tokenizer() = default;

protected:
    virtual std::optional<TokenType> GetToken(std::istream& input) = 0;
    virtual std::optional<TokenType> GetSymbol(std::istream& input);

private:
    ParserMode parser_;
};

class MapUsingTokenizers : public Tokenizer {
public:
    MapUsingTokenizers(ParserMode parser) : Tokenizer(parser) {
    }

    std::vector<CodeType> GetTokenCodes(std::istream& in) override;
    TokenType Decode(CodeType code) override;

protected:
    std::unordered_map<CodeType, TokenType> token_by_code_;
    std::unordered_map<TokenType, CodeType> code_by_token_;
};

class SymbolTokenizer : public MapUsingTokenizers {
public:
    SymbolTokenizer(ParserMode parser) : MapUsingTokenizers(parser) {
    }

protected:
    std::optional<TokenType> GetToken(std::istream& input) override;
};

template <typename IsSplitter>
class SymbolSplitTokenizer : public MapUsingTokenizers {
public:
    SymbolSplitTokenizer(ParserMode parser) : MapUsingTokenizers(parser) {
    }

protected:
    std::optional<TokenType> GetToken(std::istream& input) override;

private:
    TokenType last_read_splitter_;
};

std::unique_ptr<Tokenizer> GetTokenizer(TokenizerMode tokenizer, ParserMode parser);
