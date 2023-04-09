#include "Timer.h"

void Timer::Start() {
    _start = std::chrono::high_resolution_clock::now();
    _end = {};
}

void Timer::End() {
    _end = std::chrono::high_resolution_clock::now();
}

void Timer::Duration(std::string label) {
    if (_end == std::chrono::time_point<std::chrono::steady_clock>{}) {
        _end = std::chrono::high_resolution_clock::now();
    }
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(_end - _start);
    std::cerr << label + ": " << duration.count() << "ms\n";
}

