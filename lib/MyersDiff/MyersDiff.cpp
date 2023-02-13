#include "MyersDiff.h"

namespace Myers {

Snake::Snake(size_t begin_x, size_t begin_y, size_t width)
    : begin_x_(begin_x), begin_y_(begin_y), width_(width) {
}

std::pair<size_t, size_t> Snake::Begin() {
    return {begin_x_, begin_y_};
}

size_t Snake::Width() {
    return width_;
}

std::pair<size_t, size_t> Snake::End() {
    return {begin_x_ + width_, begin_y_ + width_};
}

}  // namespace Myers
