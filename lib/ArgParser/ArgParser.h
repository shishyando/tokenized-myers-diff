#pragma once

#include <string>
#include <map>
#include <vector>

class ArgParser {
public:
    ArgParser(int argc, char *argv[]);
    void Add(std::string opt, std::string name);
    void Build();
    int Get(std::string name) const;

private:
    std::map<std::string, int> _values;
    std::map<std::string, std::string> _aliases;
    int _argc;
    std::vector<std::string> _argv;
};

