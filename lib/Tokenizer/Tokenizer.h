#pragma once

#include <istream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <optional>
#include <unordered_set>

using TokenId = uint32_t;
using TokenType = std::string_view;

enum class ParserMode { BYTES, UTF_8 };

enum class TokenizerMode { SYMBOL, WORD, LINE, IGNORE_ALL_SPACE, IGNORE_SPACE_CHANGE, SEMANTIC_CODE };

class TokenInfo {
public:
    TokenInfo(TokenId code, uint32_t begin_pos);

    bool operator==(const TokenInfo& rhs) const;

    TokenId GetId() const;
    uint32_t GetBegin() const;

private:
    TokenId id_;
    uint32_t begin_pos_;
};

class Tokenizer {
public:
    Tokenizer(ParserMode parser, std::string_view old_data, std::string_view new_data);

    virtual std::vector<TokenInfo> GetOldTokensInfo();
    virtual std::vector<TokenInfo> GetNewTokensInfo();
    virtual TokenType DecodeOld(const TokenInfo& code);
    virtual TokenType DecodeNew(const TokenInfo& code);
    virtual ~Tokenizer() = default;

protected:
    virtual std::optional<TokenType> GetToken(std::string_view input, size_t pos) = 0;
    virtual std::optional<TokenType> GetSymbol(std::string_view input, size_t pos);
    virtual std::vector<TokenInfo> GetTokenInfos(std::string_view input) = 0;
    bool IsNewData(std::string_view data);
private:
    ParserMode parser_;
    std::string_view old_data_;
    std::string_view new_data_;
};

class MapUsingTokenizers : public Tokenizer {
public:
    MapUsingTokenizers(ParserMode parser, std::string_view old_data, std::string_view new_data);

protected:
    std::vector<TokenInfo> GetTokenInfos(const std::string_view input) override;
    virtual bool IsEqual(TokenType lhs, TokenType rhs);
    virtual size_t GetHash(TokenType token);

    std::unordered_map<size_t, TokenId> hash2id_;
    std::vector<TokenType> tokens_;
    TokenId last_code_ = 0;
};

class SymbolTokenizer : public MapUsingTokenizers {
public:
    SymbolTokenizer(ParserMode parser, std::string_view old_data, std::string_view new_data);

protected:
    std::optional<TokenType> GetToken(std::string_view input, size_t pos) override;
};

template <typename IsSplitter>
class SymbolSplitTokenizer : public MapUsingTokenizers {
public:
    SymbolSplitTokenizer(ParserMode parser, std::string_view old_data, std::string_view new_data);

protected:
    std::optional<TokenType> GetToken(std::string_view input, size_t pos) override;
};

struct IsWordSplitter {
    bool operator()(TokenType token) const;
};

struct IsLineSplitter {
    bool operator()(TokenType token) const;
};

using WordTokenizer = SymbolSplitTokenizer<IsWordSplitter>;
using LineTokenizer = SymbolSplitTokenizer<IsLineSplitter>;

class IgnoreAllSpaceTokenizer : public LineTokenizer {
public:
    IgnoreAllSpaceTokenizer(ParserMode parser, std::string_view old_data,
                            std::string_view new_data);

protected:
    bool IsEqual(TokenType lhs, TokenType rhs) override;
    size_t GetHash(TokenType token) override;
};

class IgnoreSpaceChangeTokenizer : public LineTokenizer {
public:
    IgnoreSpaceChangeTokenizer(ParserMode parser, std::string_view old_data,
                               std::string_view new_data);

protected:
    bool IsEqual(TokenType lhs, TokenType rhs) override;
    size_t GetHash(TokenType token) override;
};

class CodeTokenizer : public MapUsingTokenizers {
public:
    CodeTokenizer(std::string_view old_data, std::string_view new_data, 
            const std::string& path_to_old_file, const std::string& path_to_new_file);
private:
    std::optional<TokenType> GetToken(std::string_view input, size_t pos) override;
    std::unordered_set<size_t> parsed_old_tokens_;
    std::unordered_set<size_t> parsed_new_tokens_;
};

std::unique_ptr<Tokenizer> GetTokenizer(TokenizerMode tokenizer, ParserMode parser,
                                        std::string_view old_data, std::string_view new_data,
                                        std::string& path_to_old_file, std::string& path_to_new_file);
