#include <iostream>
#include <sstream>
#include <string>

#include "lib/ArgParser/ArgParser.h"
#include "lib/MyersDiff/MyersDiff.h"
#include "lib/Tokenizer/Tokenizer.h"

int main(int argc, char *argv[]) {
    ArgParser argParser(argc, argv);
    argParser.Add("a", "alias");
    argParser.Add("b", "blias");
    argParser.Add("c", "clias");
    argParser.Add("d", "dlias");
    argParser.Add("e", "elias");
    argParser.Build();
    std::cout << argParser.Get("alias") << '\n';
    std::cout << argParser.Get("blias") << '\n';
    std::cout << argParser.Get("clias") << '\n';
    std::cout << argParser.Get("dlias") << '\n';
    std::cout << argParser.Get("elias") << '\n';

    const std::string text1 = "Never gonna give you up, Never gonna let you down";
    const std::string text2 = "Never gona give youup, Never gonna you let down";

    std::istringstream stream1(text1);
    std::istringstream stream2(text2);
    std::unique_ptr<Tokenizer> a = GetTokenizer(TokenizerMode::WORD, ParserMode::UTF_8);
    std::vector<CodeType> vec1 = a->GetTokenCodes(stream1);
    std::vector<CodeType> vec2 = a->GetTokenCodes(stream2);

    Myers::Script script = Myers::ShortestEditScript<CodeType>(vec1, vec2);

    for (const auto& r : script) {
        std::cout << "-" << std::endl;
        for (size_t i = r.from_left; i < r.from_right; i++) {
            std::cout << a->Decode(vec1[i]) << " ";
        }
        std::cout << std::endl;
        std::cout << "+" << std::endl;
        for (size_t i = r.to_left; i < r.to_right; i++) {
            std::cout << a->Decode(vec2[i]) << " ";
        }
        std::cout << std::endl;
        std::cout << "-------------------------" << std::endl;
    }

    return 0;
}
