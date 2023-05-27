#pragma once

#include <chrono>
#include <string>
#include <iostream>

class Timer {
public:
    void Start();
    long long MsDuration(bool reset = true);

private:
    std::chrono::time_point<std::chrono::steady_clock> start_ =
        std::chrono::steady_clock::now();
    std::chrono::time_point<std::chrono::steady_clock> end_ = {};
};
