#include "DiffRunner.h"

#include <sys/fcntl.h>
#include <sys/mman.h>

#include <cstring>
#include <sstream>
#include <stdexcept>

using namespace std::literals::string_literals;

DiffRunner::DiffRunner(TokenizerMode tokenizer_mode, ParserMode parser_mode,
                       const DiffPrint::DiffPrinter& diff_printer, bool benchmark)
    : tokenizer_mode_(tokenizer_mode),
      parser_mode_(parser_mode),
      diff_printer_(diff_printer),
      benchmark_(benchmark) {
}

void DiffRunner::FileDiff(std::ostream& output, fs::path old_file_path, fs::path new_file_path) {
    fs::file_status old_file_status = fs::status(old_file_path);
    fs::file_status new_file_status = fs::status(old_file_path);
    if (old_file_status.type() == fs::file_type::not_found) {
        throw std::invalid_argument("no such file : " + old_file_path.string());
    }
    if (new_file_status.type() == fs::file_type::not_found) {
        throw std::invalid_argument("no such file : " + new_file_path.string());
    }
    if (old_file_status.type() != fs::file_type::regular) {
        throw std::invalid_argument("can't make diff with non regular file : " +
                                    old_file_path.string());
    }
    if (new_file_status.type() != fs::file_type::regular) {
        throw std::invalid_argument("can't make diff with non regular file : " +
                                    new_file_path.string());
    }

    if (benchmark_) {
        timer_.Start();
    }

    size_t old_file_size = fs::file_size(old_file_path);
    size_t new_file_size = fs::file_size(new_file_path);
    const char* old_file_mem;
    const char* new_file_mem;
    try {
        old_file_mem = MmapFile(old_file_path.string().c_str(), old_file_size);
        new_file_mem = MmapFile(new_file_path.string().c_str(), new_file_size);
    } catch (std::exception& ex) {
        throw std::runtime_error("mmap: "s + ex.what());
    }

    std::string_view old_file(old_file_mem, old_file_size);
    std::string_view new_file(new_file_mem, new_file_size);

    if (benchmark_) {
        mmap_miliseconds_ += timer_.MsDuration();
    }

    std::unique_ptr<Tokenizer> tokenizer;
    std::vector<TokenInfo> old_code, new_code;
    try {
        tokenizer = GetTokenizer(tokenizer_mode_, parser_mode_, old_file, new_file,
                                 old_file_path.string(), new_file_path.string());
        old_code = tokenizer->GetOldTokensInfo();
        new_code = tokenizer->GetNewTokensInfo();
    } catch (const std::exception& ex) {
        throw std::runtime_error("Tokenizer error: "s + ex.what());
    }

    if (benchmark_) {
        tokenize_miliseconds_ = timer_.MsDuration();
    }

    Myers::Script script;
    try {
        script = Myers::ShortestEditScript(old_code, new_code);
    } catch (const std::exception& ex) {
        throw std::runtime_error("Myers Algorithm error: "s + ex.what());
    }

    if (benchmark_) {
        diff_miliseconds_ += timer_.MsDuration();
    }

    try {
        diff_printer_.Print(output, script, tokenizer, old_code, new_code);
    } catch (const std::exception& ex) {
        throw std::runtime_error("Print Diff eror: "s + ex.what());
    }

    if (benchmark_) {
        print_miliseconds_ += timer_.MsDuration("print");
    }
}

void DiffRunner::DirDiff(std::ostream& output, fs::path old_dir_path, fs::path new_dir_path) {
    if (fs::status(old_dir_path).type() != fs::file_type::directory) {
        throw std::invalid_argument("No such directory: " + old_dir_path.string());
    }
    if (fs::status(new_dir_path).type() != fs::file_type::directory) {
        throw std::invalid_argument("No such directory: " + new_dir_path.string());
    }
    for (auto& dir_entry : fs::recursive_directory_iterator{old_dir_path}) {
        try {
            fs::path old_file_path = dir_entry.path();
            fs::path rel_path = fs::relative(old_file_path, old_dir_path);
            fs::path new_file_path = new_dir_path / rel_path;

            fs::file_status old_file_stat = fs::status(old_file_path);
            fs::file_status new_file_stat = fs::status(new_file_path);
            if (new_file_stat.type() == fs::file_type::not_found) {
                output << "Only in " << new_file_path.parent_path().string() << " : "
                       << new_file_path.filename().string() << '\n';
            } else if (old_file_stat.type() != new_file_stat.type()) {
                output << "File " << old_file_path.string() << " is a "
                       << FileTypeToString(old_file_stat.type()) << " while "
                       << new_file_path.string() << " is a "
                       << FileTypeToString(new_file_stat.type()) << '\n';
            } else if (old_file_stat.type() != fs::file_type::directory) {
                std::stringstream sstream;
                FileDiff(sstream, old_file_path, new_file_path);
                if (sstream.str().size() != 1) {
                    output << old_file_path.string() << " and " << new_file_path.string()
                           << " diff:\n"
                           << sstream.str();
                }
            }
        } catch (std::exception& ex) {
            std::cerr << ex.what() << '\n';
        }
    }

    for (auto& dir_entry : fs::recursive_directory_iterator{new_dir_path}) {
        fs::path new_file_path = dir_entry.path();
        fs::path rel_path = fs::relative(new_file_path, new_dir_path).string();
        fs::path old_file_path = old_dir_path / rel_path;

        fs::file_status old_file_stat = fs::status(old_file_path);
        if (old_file_stat.type() == fs::file_type::not_found) {
            output << "Only in " << old_file_path.parent_path().string() << " : "
                   << old_file_path.filename().string() << '\n';
        }
    }
}

long long DiffRunner::GetMmapMs() {
    return mmap_miliseconds_;
}

long long DiffRunner::GetTokenizeMs() {
    return tokenize_miliseconds_;
}

long long DiffRunner::GetDiffMs() {
    return diff_miliseconds_;
}

long long DiffRunner::GetPrintMs() {
    return print_miliseconds_;
}

const char* DiffRunner::MmapFile(const char* file_path, size_t file_size) {
    if (file_size == 0) {
        return nullptr;
    }
    int fd = open(file_path, O_RDONLY);
    errno = 0;
    void* mem = mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mem == MAP_FAILED) {
        throw std::runtime_error(std::strerror(errno));
    }
    return static_cast<const char*>(mem);
}

std::string DiffRunner::FileTypeToString(fs::file_type type) {
    switch (type) {
        case fs::file_type::none:
            return "`not-evaluated-yet` type file";
        case fs::file_type::not_found:
            return "non-existent file";
        case fs::file_type::regular:
            return "regular file";
        case fs::file_type::directory:
            return "directory";
        case fs::file_type::symlink:
            return "symlink";
        case fs::file_type::block:
            return "block device";
        case fs::file_type::character:
            return "character device";
        case fs::file_type::fifo:
            return "IPC pipe";
        case fs::file_type::socket:
            return "IPC socket";
        default:
            return "`unknown` type file";
    }
}
