#include "Timer.h"

void Timer::Start() {
    start_ = std::chrono::steady_clock::now();
    end_ = {};
}

void Timer::Duration(std::string label, bool reset) {
    if (end_ == std::chrono::time_point<std::chrono::steady_clock>{}) {
        end_ = std::chrono::steady_clock::now();
    }
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_ - start_);
    std::cerr << label + ": " << duration.count() << "ms\n";
    if (reset) {
        start_ = std::chrono::steady_clock::now();
        end_ = std::chrono::time_point<std::chrono::steady_clock>{};
    }
}
