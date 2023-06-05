#include "LSCommunicator.h"
#include "lib/JsonParser/json.hpp"

#include <QCoreApplication>
#include <QProcess>
#include <QString>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <cstdlib>

namespace LSCommunicator {
const std::string kConfigFilePath = "/.diff_config.json";
const int kMaxMsgToRead = 10;  // If we don't find data in 10 message,
                               // we will return with empty result.

static std::string ReadFileContent(const std::string& full_file_path) {
    std::ifstream file(full_file_path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

static std::string SendMessage(QProcess* server, const std::string& content) {
    std::ostringstream oss;
    oss << "Content-Length: " << content.length() << "\n"
        << "\n";
    std::string header = oss.str();
    server->write(header.c_str());
    server->write(content.c_str());
    server->waitForReadyRead(-1);
    return server->readAll().toStdString();
}

static QProcess* MakeServer([[maybe_unused]] const std::string& file_path,
                            const std::string& ls_path) {
    QProcess* server = new QProcess{};
    const QString server_name_qstring(ls_path.c_str());
    server->start(server_name_qstring, QStringList{});
    server->waitForStarted(-1);
    return server;
}

static void InitServer(QProcess* server, const std::string& cur_dir) {
    nlohmann::json init_req;
    init_req["jsonrpc"] = "2.0";
    init_req["id"] = 0;
    init_req["method"] = "initialize";
    init_req["params"]["processId"] = 0;
    init_req["params"]["rootUri"] = cur_dir;
    init_req["capabilities"] = nlohmann::json::array();
    SendMessage(server, init_req.dump());
}

static void NotifyDidOpen(QProcess* server, const std::string& full_file_path,
                          const std::string& file_content) {
    nlohmann::json did_open_req;
    did_open_req["jsonrpc"] = "2.0";
    did_open_req["method"] = "textDocument/didOpen";
    did_open_req["params"]["textDocument"]["uri"] = "file://" + full_file_path;
    did_open_req["params"]["textDocument"]["languageId"] = "cpp";
    did_open_req["params"]["textDocument"]["version"] = 0;
    did_open_req["params"]["textDocument"]["text"] = file_content;
    SendMessage(server, did_open_req.dump());
}

static std::string RequestParseFile(QProcess* server, const std::string& full_file_path) {
    nlohmann::json parse_file_req;
    parse_file_req["id"] = 0;
    parse_file_req["jsonrpc"] = "2.0";
    parse_file_req["method"] = "textDocument/semanticTokens/full";
    parse_file_req["params"]["textDocument"]["uri"] = "file://" + full_file_path;
    return SendMessage(server, parse_file_req.dump());
}

static std::vector<size_t> GetResponse(std::string& msg) {
    size_t json_start = 0;
    while (json_start < msg.length() && msg[json_start] != '{') {
        ++json_start;
    }
    msg.erase(0, json_start);
    nlohmann::json res = nlohmann::json::parse(msg);
    if (res.contains("result")) {
        if (res["result"].contains("data")) {
            return res["result"]["data"].get<std::vector<size_t>>();
        }
    }
    throw std::runtime_error("Language Server doesn't support semantic tokenization");
}

static std::vector<size_t> GetTokensPos(const std::vector<size_t>& data,
                                        const std::string& file_content) {
    size_t prev_lines_len = 0, prev_pos = 0;
    std::vector<size_t> tokens;
    for (size_t i = 0; i < data.size(); i += 5) {
        size_t line = data[i], pos = data[i + 1], len = data[i + 2];
        if (line != 0) {
            prev_pos = 0;
            for (size_t _ = 0; _ < line; ++_) {
                prev_lines_len = file_content.find("\n", prev_lines_len) + 1;
            }
        }
        pos += prev_pos;
        prev_pos = pos;
        pos += prev_lines_len;
        tokens.push_back(pos);
        tokens.push_back(pos + len);
    }
    return tokens;
}


static std::string GetLSPath(const std::string& file_path) {
    std::string file_ext = std::filesystem::path(file_path).extension();
    std::string full_path = std::getenv("HOME") + kConfigFilePath;
    std::ifstream config_file;
    config_file.open(full_path);
    if (!config_file.is_open()) {
        throw std::runtime_error("Unable to read config file for Language Server on \"" + full_path + "\"");
    }
    nlohmann::json config;
    config_file >> config;
    for (const auto& config_line : config["LS"].items()) {
        auto LS = config_line.value();
        for (const auto& ext : LS["extensions"].items()) {
            if (ext.value() == file_ext) {
                return LS["name"];
            }
        }
    }
    throw std::runtime_error("Couldn't find Language Server for " + file_ext + " extension");
}

std::unordered_set<size_t> GetParseResult(const std::string& file_path) {
    std::string ls_path = GetLSPath(file_path);
    QProcess* server = MakeServer(file_path, ls_path);

    std::string cur_dir = std::filesystem::current_path();
    std::string full_file_path = cur_dir + "/" + file_path;
    std::string file_content = ReadFileContent(full_file_path);

    InitServer(server, cur_dir);
    NotifyDidOpen(server, full_file_path, file_content);
    std::string res = RequestParseFile(server, full_file_path);
    std::vector<size_t> tokens = GetTokensPos(GetResponse(res), file_content);
    server->kill();
    return std::unordered_set<size_t>(tokens.begin(), tokens.end());
}

}  // namespace LSCommunicator
