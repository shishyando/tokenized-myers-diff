#pragma once
#include <vector>
#include <memory>
#include <string>


using TokenType = int;

enum TokenizersTypes {
    DEFAULT, // by symbols
    SPACE_SPLIT, // splits by spaces
    /// TODO...
};

template <typename TextType>
class Tokenizer {
public:
    explicit Tokenizer() = default;
    virtual std::vector<TokenType> GetTokens() = 0;
    virtual ~Tokenizer() = default;
};

template <typename TextType>
class SplitTokenizer final : public Tokenizer<TextType> {
public:
    SplitTokenizer(const TextType& text, TokenType splitter) : _text(text), _splitter(splitter) {}
    std::vector<TokenType> GetTokens() override;
private:
    std::size_t GetNextTokenPos(size_t cur_pos = -1);
    bool IsSameType(TokenType a, TokenType b);
    const TextType& _text;
    TokenType _splitter;
};

template <typename TextType>
class SymbolTokenizer final : public Tokenizer<TextType> {
public:
    explicit SymbolTokenizer(const TextType& text) : _text(text) {}
    std::vector<TokenType> GetTokens() override;
private:
    const TextType& _text;
};

template <typename TextType>
std::unique_ptr<Tokenizer<TextType>> GetTokenizer(TokenizersTypes type, const TextType& text);