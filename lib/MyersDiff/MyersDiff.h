#pragma once

#include <vector>
#include <utility>
#include <stdexcept>

namespace Myers {

struct Replacement {
    uint32_t from_left, from_right;
    uint32_t to_left, to_right;
};

using Script = std::vector<Replacement>;

class Snake {
public:
    Snake(uint32_t begin_x, uint32_t begin_y, uint32_t width);

    std::pair<uint32_t, uint32_t> Begin();
    uint32_t Width();
    std::pair<uint32_t, uint32_t> End();

private:
    uint32_t begin_x_;
    uint32_t begin_y_;
    uint32_t width_;
};

template <typename T>
static std::pair<uint32_t, Snake> GetMiddleSnake(const std::vector<T>& from,
                                                 const std::vector<T>& to, uint32_t from_left,
                                                 uint32_t from_right, uint32_t to_left,
                                                 uint32_t to_right) {
    uint32_t from_size = from_right - from_left;
    uint32_t to_size = to_right - to_left;

    uint32_t total_size = from_size + to_size;
    int32_t delta = from_size - to_size;
    uint32_t offset = total_size + 1 + abs(delta);
    bool is_odd = total_size & 1;

    std::vector<uint32_t> max_direct_path(offset * 2);
    std::vector<int32_t> max_reversed_path(offset * 2, from_size + 1);

    for (uint32_t script_size = 0; script_size <= (total_size + 1) / 2; ++script_size) {
        uint32_t lowest_diag = offset - script_size;
        uint32_t highest_diag = offset + script_size;
        for (uint32_t diagonal = lowest_diag; diagonal <= highest_diag; diagonal += 2) {
            uint32_t from_id;
            if (diagonal == lowest_diag ||
                (diagonal != highest_diag &&
                 max_direct_path[diagonal - 1] < max_direct_path[diagonal + 1])) {
                from_id = max_direct_path[diagonal + 1];
            } else {
                from_id = max_direct_path[diagonal - 1] + 1;
            }

            uint32_t to_id = from_id + offset - diagonal + to_left;
            from_id += from_left;
            uint32_t snake_length = 0;
            while (from_id < from_right && to_id < to_right && from[from_id] == to[to_id]) {
                ++from_id;
                ++to_id;
                ++snake_length;
            }
            max_direct_path[diagonal] = from_id - from_left;

            if (is_odd && diagonal >= lowest_diag + delta + 1 &&
                diagonal <= highest_diag + delta - 1 &&
                static_cast<int32_t>(max_direct_path[diagonal]) >= max_reversed_path[diagonal]) {
                return {script_size * 2 - 1,
                        {from_id - snake_length, to_id - snake_length, snake_length}};
            }
        }

        lowest_diag += delta;
        highest_diag += delta;
        for (uint32_t diagonal = lowest_diag; diagonal <= highest_diag; diagonal += 2) {
            int32_t from_id;
            if (diagonal == lowest_diag ||
                (diagonal != highest_diag &&
                 max_reversed_path[diagonal + 1] <= max_reversed_path[diagonal - 1])) {
                from_id = max_reversed_path[diagonal + 1] - 1;
            } else {
                from_id = max_reversed_path[diagonal - 1];
            }

            int32_t to_id = from_id + offset - diagonal + to_left;
            from_id += from_left;
            uint32_t snake_length = 0;
            while (from_id > static_cast<int32_t>(from_left) &&
                   to_id > static_cast<int32_t>(to_left) && from[from_id - 1] == to[to_id - 1]) {
                --from_id;
                --to_id;
                ++snake_length;
            }
            max_reversed_path[diagonal] = from_id - from_left;

            if (!is_odd && diagonal >= lowest_diag - delta && diagonal <= highest_diag - delta &&
                static_cast<int32_t>(max_direct_path[diagonal]) >= max_reversed_path[diagonal]) {
                return {
                    script_size * 2,
                    {static_cast<uint32_t>(from_id), static_cast<uint32_t>(to_id), snake_length}};
            }
        }
    }
    throw std::logic_error("SES haven't been found");
}

template <typename T>
static void GetSnakeDecomposition(const std::vector<T>& from, const std::vector<T>& to,
                                  uint32_t from_left, uint32_t from_right, uint32_t to_left,
                                  uint32_t to_right, std::vector<int32_t>& from_snake,
                                  std::vector<int32_t>& to_snake, uint32_t& current_snake) {
    auto [ses_size, snake] = GetMiddleSnake(from, to, from_left, from_right, to_left, to_right);
    if (ses_size > 1) {
        auto [from_begin, to_begin] = snake.Begin();
        auto [from_end, to_end] = snake.End();
        GetSnakeDecomposition(from, to, from_left, from_begin, to_left, to_begin, from_snake,
                              to_snake, current_snake);
        for (uint32_t id = 0; id < snake.Width(); ++id) {
            from_snake[from_begin + id] = to_snake[to_begin + id] = current_snake;
        }
        if (snake.Width() > 0) {
            ++current_snake;
        }
        GetSnakeDecomposition(from, to, from_end, from_right, to_end, to_right, from_snake,
                              to_snake, current_snake);
    } else if (from_right - from_left < to_right - to_left) {
        if (from_right == from_left) {
            return;
        }
        int32_t shift = 0;
        for (uint32_t id = 0; id < from_right - from_left; ++id) {
            if (!(from[from_left + id] == to[to_left + id + shift])) {
                ++current_snake;
                ++shift;
            }
            from_snake[from_left + id] = to_snake[to_left + id + shift] = current_snake;
        }
        ++current_snake;
    } else {
        if (to_right == to_left) {
            return;
        }
        int32_t shift = 0;
        for (uint32_t id = 0; id < to_right - to_left; ++id) {
            if (!(from[from_left + id + shift] == to[to_left + id])) {
                ++current_snake;
                ++shift;
            }
            from_snake[from_left + id + shift] = to_snake[to_left + id] = current_snake;
        }
        ++current_snake;
    }
}

template <typename T>
std::vector<T> LargestCommonSubsequence(const std::vector<T>& from, const std::vector<T>& to) {
    if (from.empty() || to.empty()) {
        return {};
    }
    std::vector<int32_t> from_snake(from.size(), -1);
    std::vector<int32_t> to_snake(to.size(), -1);
    uint32_t current_snake = 0;
    GetSnakeDecomposition(from, to, 0, from.size(), 0, to.size(), from_snake, to_snake,
                          current_snake);
    std::vector<T> lcs;
    for (uint32_t i = 0; i < from.size(); ++i) {
        if (from_snake[i] != -1) {
            lcs.emplace_back(from[i]);
        }
    }
    return lcs;
}

template <typename T>
Script ShortestEditScript(const std::vector<T>& from, const std::vector<T>& to) {
    if (from.empty()) {
        if (!to.empty()) {
            return {Replacement{0, 0, 0, static_cast<uint32_t>(to.size())}};
        }
        return {};
    }
    if (to.empty()) {
        return {Replacement{0, static_cast<uint32_t>(from.size()), 0, 0}};
    }
    std::vector<int32_t> from_snake(from.size(), -1);
    std::vector<int32_t> to_snake(to.size(), -1);
    uint32_t current_snake = 0;
    GetSnakeDecomposition(from, to, 0, from.size(), 0, to.size(), from_snake, to_snake,
                          current_snake);
    Script script;
    uint32_t to_id = 0;
    for (uint32_t from_id = 0; from_id < from.size(); ++from_id, ++to_id) {
        uint32_t from_left = from_id;
        while (from_id < from.size() && from_snake[from_id] == -1) {
            ++from_id;
        }
        uint32_t to_left = to_id;
        while (to_id < to.size() &&
               (from_id == from.size() || to_snake[to_id] != from_snake[from_id])) {
            ++to_id;
        }
        script.emplace_back(Replacement{from_left, from_id, to_left, to_id});
        while (from_id + 1 < from.size() && from_snake[from_id + 1] == from_snake[from_id]) {
            ++from_id;
        }
        while (to_id + 1 < to.size() && to_snake[to_id + 1] == to_snake[to_id]) {
            ++to_id;
        }
    }
    if (to_id < to.size()) {
        script.emplace_back(Replacement{static_cast<uint32_t>(from.size()),
                                        static_cast<uint32_t>(from.size()), to_id,
                                        static_cast<uint32_t>(to.size())});
    }
    return script;
}

}  // namespace Myers
