#include "IterableObject.h"

template <typename TextType>
size_t IterableObjectHasher<TextType>::operator()(const IterableObject<TextType> &k) const {
    size_t hash = 0;
    for (size_t i = k.start; i < k.end; ++i) {
        type num = static_cast<type>(static_cast<unsigned char>(k.object[i]));
        hash ^= ((num << 12) + (num * 90087) + (num >> 13));
        hash += 13 * num + 61723;
    }
    return hash;
}

template <typename TextType>
bool IterableObject<TextType>::operator ==(const IterableObject<TextType>& other) const {
    if (end - start != other.end - other.start) {
        return false;
    }
    for (int i = 0; i < end - start; ++i) {
        if (object[i + start] != other.object[i + other.start]) {
            return false;
        }
    }
    return true;
}