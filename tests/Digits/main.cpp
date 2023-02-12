#include "lib/ArgParser/ArgParser.h"
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
    ArgParser args(argc, argv);
    args.Add("n", "lines");
    args.Add("m", "length");
    args.Add("s", "seed");
    args.Add("d", "spaces");
    args.Build();
    DigitGen gen;
    gen.Seed(args.Get("seed"));
    gen.GenTest(args.Get("lines"),
                args.Get("length"),
                args.Get("spaces"));
    return 0;
}
