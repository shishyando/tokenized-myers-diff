#pragma once
#include <chrono>
#include <string>
#include <iostream>

class Timer
{
public:
    void Start();
    void End();
    void Duration(std::string, bool reset = true);

private:
    std::chrono::time_point<std::chrono::steady_clock> _start = std::chrono::high_resolution_clock::now();
    std::chrono::time_point<std::chrono::steady_clock> _end = {};
};