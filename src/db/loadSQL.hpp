#include <fstream>
#include <sstream>
#include <string>

static std::string loadSQL(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Nie można otworzyć pliku: " + filename);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}