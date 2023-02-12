#include <iostream>
#include <string>
#include "lib/ArgParser/ArgParser.h"

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
    return 0;
}