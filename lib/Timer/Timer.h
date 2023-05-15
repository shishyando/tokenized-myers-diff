#pragma once
#include <chrono>
#include <string>
#include <iostream>

class Timer {
public:
    void Start();
    void Duration(std::string, bool reset = true);

private:
    std::chrono::time_point<std::chrono::steady_clock> start_ =
        std::chrono::high_resolution_clock::now();
    std::chrono::time_point<std::chrono::steady_clock> end_ = {};
};
