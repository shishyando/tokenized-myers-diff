# tokenized-myers-diff

The C++ program for efficient comparison of text files. The program implements the Myers algorithm and provides the ability to choose a tokenizer, as well as an output diff format.

This project is a course work of the HSE CS AMI.

## Installation

Run `cmake .` and `make`. The result is executable file `diff`.

## Usage

`diff [--help] [--version] [--parser PARSER_MODE] [--tokenizer TOKENIZER_TYPE] [--common-prefix VAR] [--old-prefix VAR] [--new-prefix VAR] [--benchmark] [--common] [--raw] [--show-common-newlines] [--show-pos] [--recursive] old new`

### Positional arguments:

  `old`                                   old file path
  
  `new`                                  new file path

### Optional arguments:

  `-h, --help`                            shows help message and exits
  
  `-v, --version`                         prints version information and exits
  
  `-p, --parser PARSER_MODE`              parser mode: bytes, utf-8 [default: "utf-8"]
  
  `-t, --tokenizer TOKENIZER_TYPE`        tokenizer type: symbol, word, line, ignore-all-space, ignore-space-change, semantic-code [default: "line"]
  
  `--common-prefix`                       prefix for printing common part in raw mode [default: ""]
  
  `--old-prefix`                          prefix for printing deleted part in raw mode [default: "[-]"]
  
  `--new-prefix`                          prefix for printing added part in raw mode [default: "[+]"]
  
  `-b, --benchmark`                       measure timings
  
  `-c, --common`                          print common parts of the files
  
  `--raw`                                 print diff without colors
  
  `--show-common-newlines`                print common part with newlines displayed as '\n'
  
  `--show-pos`                            print diff start position in both files
  
  `-r, --recursive`                       recursive directory mode

## Semantic code tokenization
Use `--tokenizer semantic-code` option to tokenize code files using [Language Sever](https://microsoft.github.io/language-server-protocol/):
![semantic code example](media/code-semantic.gif)

To install all packages for this option, you can run `sudo ./setup.sh` in you terminal. Default clangd Language Server and packages will be installed, and config file will be saved on ~/.diff_config.json. You can add more Language Server by installing them and adding to the config.
