#include "lib/argparse/argparse.hpp"
#include "tests/TestGenerator.h"

class DigitGen final : public TestGenerator {
public:
    std::string GetToken() final {
        std::string token;
        token += char(_rng() % 10 + '0');
        return token;
    }
};

int main(int argc, char *argv[]) {
    std::cout << "todo";
    return 0;
}
