#pragma once
#include <string>

//using TextType = std::string;
using type = unsigned long long; // for UTF-8 we need 4 bytes
                                 // so this will handle all cases
                                 // even negative.
template <typename TextType>
class IterableObject{
public:
    IterableObject(const TextType& object,
                    size_t start,
                    size_t end) :
            object(object),
            start(start),
            end(end) {}
    const TextType& object;
    std::size_t start, end;
    bool operator ==(const IterableObject& other) const;
};

template <typename TextType>
struct IterableObjectHasher {
    std::size_t operator()(const IterableObject<TextType>& k) const;
};

