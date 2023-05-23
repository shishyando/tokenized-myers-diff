#include "LSCommunicator.h"

#include <iostream>
#include <set>
int main() {
    auto res = ls_communicator::GetParseResult("../lib/LSCommunicator/tests/source_code.cpp");
    std::string source_code = "int main() {\n    int a;\n    int b = a + 4;\n}";
    std::set<size_t> s(res.begin(), res.end());
    size_t prev = 0;
    for (auto i : s) {
        std::cout << source_code.substr(prev, i - prev) << std::endl;
        std::cout << "_____" << std::endl;
        prev = i;
    }
    std::cout << std::endl;
}
