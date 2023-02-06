#include "Tokenizer.cpp"
#include "IterableObject/IterableObject.cpp"
#include <vector>

using TextType = std::string;
template std::size_t IterableObjectHasher<TextType>::operator()(const IterableObject<TextType> &k) const;
template bool IterableObject<TextType>::operator ==(const IterableObject<TextType>& other) const;
template std::unique_ptr<Tokenizer<TextType>> GetTokenizer(TokenizersTypes type, const TextType& text);
template std::vector<TokenType> SymbolTokenizer<TextType>::GetTokens();
template std::vector<TokenType> SplitTokenizer<TextType>::GetTokens();

using TextType1 = std::vector<char>;
template std::size_t IterableObjectHasher<TextType1>::operator()(const IterableObject<TextType1> &k) const;
template bool IterableObject<TextType1>::operator ==(const IterableObject<TextType1>& other) const;
template std::unique_ptr<Tokenizer<TextType1>> GetTokenizer(TokenizersTypes type, const TextType1& text);
template std::vector<TokenType> SymbolTokenizer<TextType1>::GetTokens();
template std::vector<TokenType> SplitTokenizer<TextType1>::GetTokens();