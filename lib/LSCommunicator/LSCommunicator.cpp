#include "LSCommunicator.h"
#include "lib/JsonParser/json.hpp"

#include <QCoreApplication>
#include <QProcess>
#include <QString>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <vector>

namespace LSCommunicator {
const int kMaxMsgToRead = 10;  // If we don't find data in 10 message,
                               // we will return with empty result.

static std::string ReadFileContent(const std::string& full_file_path) {
    std::ifstream file(full_file_path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

static void SendMessage(QProcess* server, const std::string content) {
    std::ostringstream oss;
    oss << "Content-Length: " << content.length() << "\n"
        << "\n";
    std::string header = oss.str();
    server->write(header.c_str());
    server->write(content.c_str());
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

static void RequestParseFile(QProcess* server, const std::string& full_file_path) {
    nlohmann::json parse_file_req;
    parse_file_req["id"] = 0;
    parse_file_req["jsonrpc"] = "2.0";
    parse_file_req["method"] = "textDocument/semanticTokens/full";
    parse_file_req["params"]["textDocument"]["uri"] = "file://" + full_file_path;
    SendMessage(server, parse_file_req.dump());
}

static nlohmann::json GetJsonFromServer(QProcess* server) {
    server->waitForReadyRead(-1);
    std::string msg = server->readAll().toStdString();
    size_t json_start = 0;
    while (json_start < msg.length() && msg[json_start] != '{') {
        ++json_start;
    }
    msg.erase(0, json_start);
    return nlohmann::json::parse(msg);
}

static std::vector<size_t> FindResponse(QProcess* server) {
    for (int i = 0; i < kMaxMsgToRead; i++) {
        nlohmann::json res = GetJsonFromServer(server);
        if (res.contains("result")) {
            if (res["result"].contains("data")) {
                return res["result"]["data"].get<std::vector<size_t>>();
            }
        }
    }
    return std::vector<size_t>{};
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

std::string GetLSPath([[maybe_unused]] const std::string& file_path) {
#if defined(__APPLE__) || defined(__MACH__)
    return "clangd";  // To extend later.
#elif __linux__
    return "clangd-12"
#else
    throw std::runtime_error("Unsupported platform");
#endif
}

std::unordered_set<size_t> GetParseResult(const std::string& file_path) {
    std::string ls_path = GetLSPath(file_path);
    QProcess* server = MakeServer(file_path, ls_path);

    std::string cur_dir = std::filesystem::current_path();
    std::string full_file_path = cur_dir + "/" + file_path;
    std::string file_content = ReadFileContent(full_file_path);

    InitServer(server, cur_dir);
    NotifyDidOpen(server, full_file_path, file_content);
    RequestParseFile(server, full_file_path);
    std::vector<size_t> tokens = GetTokensPos(FindResponse(server), file_content);
    server->kill();
    return std::unordered_set<size_t>(tokens.begin(), tokens.end());
}

}  // namespace LSCommunicator
