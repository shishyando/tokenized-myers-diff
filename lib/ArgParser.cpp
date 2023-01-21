#include "ArgParser.h"

ArgParser::ArgParser(int argc, char *argv[]) : _argc(argc) {
    for (int i = 0; i < argc; ++i) {
        _argv.emplace_back(argv[i]);
    }
};

void ArgParser::Add(std::string opt, std::string name) {
    _aliases[opt] = name;
}

void ArgParser::Build() {
    for (int i = 1; i < _argc; ++i) {
        std::string s = std::string(_argv[i]);
        if (s.find("=") != std::string::npos) {
            auto pos = s.find("=");
            _values[_aliases[s.substr(0, pos)]] = std::stoi(s.substr(pos + 1));
        } else {
            _values[_aliases[s]] = 1;
        }
    }
}

int ArgParser::Get(std::string name) const {
    try {
       return _values.at(name);
    } catch (...) {
        return 0;
    }
}
