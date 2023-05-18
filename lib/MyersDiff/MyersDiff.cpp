#include "MyersDiff.h"

namespace Myers {

Snake::Snake(uint32_t begin_x, uint32_t begin_y, uint32_t width)
    : begin_x_(begin_x), begin_y_(begin_y), width_(width) {
}

std::pair<uint32_t, uint32_t> Snake::Begin() {
    return {begin_x_, begin_y_};
}

uint32_t Snake::Width() {
    return width_;
}

std::pair<uint32_t, uint32_t> Snake::End() {
    return {begin_x_ + width_, begin_y_ + width_};
}

}  // namespace Myers
