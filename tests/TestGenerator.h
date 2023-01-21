#pragma once

#include <random>
#include <string>
#include <iostream>

class TestGenerator {
public:
    virtual void GenTest(int lines, int length, bool spaces) {
        std::string tests[2];
        for (int test_id = 0; test_id < 2; ++test_id) {
            for (int i = 0; i < lines; ++i) {
                for (int j = 0; j < length; ++j) {
                    tests[test_id] += this->GetToken();
                    if (spaces) {
                        tests[test_id] += ' ';
                    }
                }
                tests[test_id] += '\n';
            }
        }
        std::cout << tests[0];
        std::cerr << tests[1];
    }

    void Seed(int seed) {
        _seed = seed;
        _rng.seed(seed);
    }

protected:
    virtual std::string GetToken() {
        return "GetToken not overwritten\n";
    }

    int _seed;
    std::mt19937 _rng;
};
