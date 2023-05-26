#include <string>
#include <optional>
#include <unordered_set>

namespace LSCommunicator {
/*
 * Parses file using language server(LS).
 *
 * file_path - path to the file (will be resolved according to the current directory)
 *
 * return: std::unordered_set<size_t> with the tokens borders.
 */

std::unordered_set<size_t> GetParseResult(const std::string& file_path);

}  // namespace LSCommunicator
