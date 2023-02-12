#pragma once
#include <istream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

using CodeType = size_t;
using TokenType = std::string;

enum ParserType { BY_CHARS = 0, BY_UTF_8 };

enum TokenizersType {
    DEFAULT,      // by symbols
    SPACE_SPLIT,  // splits by spaces
    /// TODO...
};

class Tokenizer {
   public:
    explicit Tokenizer(std::istream& stream, bool is_utf_8 = false)
        : _stream(stream), _is_utf_8(is_utf_8) {}
    virtual std::vector<CodeType> GetTokensCodes() = 0;
    virtual TokenType Decode(CodeType code) = 0;
    virtual ~Tokenizer() = default;

   protected:
    bool _GetNextSymbol(TokenType& c);

   private:
    std::istream& _stream;
    bool _is_utf_8;
};

class MapUsingTokenizers : public Tokenizer {
   public:
    MapUsingTokenizers(std::istream& stream, bool is_utf_8 = false)
        : Tokenizer(stream, is_utf_8) {}
    std::vector<CodeType> GetTokensCodes() override;
    TokenType Decode(CodeType code) override;

   protected:
    std::unordered_map<CodeType, TokenType> _code_to_token;
    virtual bool _GetNextToken(TokenType& token) = 0;
};

class SplitTokenizer final : public MapUsingTokenizers {
   public:
    SplitTokenizer(TokenType splitter, std::istream& stream,
                   bool is_utf_8 = false)
        : MapUsingTokenizers(stream, is_utf_8), _splitter(splitter) {}

   private:
    bool _GetNextToken(TokenType& token) override;
    TokenType _splitter;
};

class SymbolTokenizer final : public MapUsingTokenizers {
   public:
    explicit SymbolTokenizer(std::istream& stream, bool is_utf_8 = false)
        : MapUsingTokenizers(stream, is_utf_8) {}
    bool _GetNextToken(TokenType& token) override;
};

std::unique_ptr<Tokenizer> GetTokenizer(TokenizersType tokenizer,
                                        ParserType parser,
                                        std::istream& stream);
