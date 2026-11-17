#include "Utils.h"
#include <iostream>
#include <cstdlib>
#include <cerrno>
#include <climits>
#include <cmath>
#include <ctime>
#include <sstream>

namespace Utils {

bool parseInt(const std::string& s, int& out) {
    if (s.empty()) return false;
    char* end = nullptr;
    errno = 0;
    long v = std::strtol(s.c_str(), &end, 10);
    if (errno != 0 || *end != '\0' || v < INT_MIN || v > INT_MAX) return false;
    out = static_cast<int>(v);
    return true;
}

bool parseDouble(const std::string& s, double& out) {
    if (s.empty()) return false;
    char* end = nullptr;
    errno = 0;
    double v = std::strtod(s.c_str(), &end);
    if (errno != 0 || *end != '\0' || !std::isfinite(v)) return false;
    out = v;
    return true;
}

std::string trim(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> parts;
    std::string cur;
    std::istringstream ss(s);
    while (std::getline(ss, cur, delim)) parts.push_back(cur);
    if (!s.empty() && s.back() == delim) parts.push_back("");
    return parts;
}

bool isValidName(const std::string& s) {
    if (trim(s).empty()) return false;
    return s.find_first_of("|\r\n") == std::string::npos;
}

std::string readLine(const std::string& prompt) {
    std::cout << prompt << std::flush;
    std::string line;
    if (!std::getline(std::cin, line)) {
        std::cout << "\n[Kết thúc nhập liệu]\n";
        std::exit(0);
    }
    return line;
}

int readInt(const std::string& prompt, int minVal, int maxVal) {
    while (true) {
        int v;
        if (parseInt(trim(readLine(prompt)), v) && v >= minVal && v <= maxVal) return v;
        std::cout << "  ! Giá trị không hợp lệ (cần số nguyên từ " << minVal
                  << " đến " << maxVal << ").\n";
    }
}

double readDouble(const std::string& prompt, double minExclusive) {
    while (true) {
        double v;
        if (parseDouble(trim(readLine(prompt)), v) && v > minExclusive) return v;
        std::cout << "  ! Giá trị không hợp lệ (cần số lớn hơn " << minExclusive << ").\n";
    }
}

bool readOptionalInt(const std::string& prompt, int minVal, int maxVal, int& out) {
    while (true) {
        std::string s = trim(readLine(prompt));
        if (s.empty()) return false;
        int v;
        if (parseInt(s, v) && v >= minVal && v <= maxVal) { out = v; return true; }
        std::cout << "  ! Không hợp lệ (Enter để bỏ qua).\n";
    }
}

bool readOptionalDouble(const std::string& prompt, double minExclusive, double& out) {
    while (true) {
        std::string s = trim(readLine(prompt));
        if (s.empty()) return false;
        double v;
        if (parseDouble(s, v) && v > minExclusive) { out = v; return true; }
        std::cout << "  ! Không hợp lệ (Enter để bỏ qua).\n";
    }
}

bool confirm(const std::string& prompt) {
    while (true) {
        std::string s = trim(readLine(prompt + " (y/n): "));
        if (s == "y" || s == "Y") return true;
        if (s == "n" || s == "N") return false;
        std::cout << "  ! Chỉ nhập y hoặc n.\n";
    }
}

std::string formatMoney(double v) {
    long long x = std::llround(v);
    bool neg = x < 0;
    if (neg) x = -x;
    std::string digits = std::to_string(x), out;
    int cnt = 0;
    for (int i = static_cast<int>(digits.size()) - 1; i >= 0; --i) {
        out.insert(out.begin(), digits[i]);
        if (++cnt % 3 == 0 && i > 0) out.insert(out.begin(), ',');
    }
    return (neg ? "-" : "") + out + " d";
}

static size_t utf8Length(const std::string& s) {
    size_t n = 0;
    for (unsigned char c : s) if ((c & 0xC0) != 0x80) ++n;
    return n;
}

std::string padRight(const std::string& s, size_t width) {
    size_t len = utf8Length(s);
    return len >= width ? s : s + std::string(width - len, ' ');
}

std::string padLeft(const std::string& s, size_t width) {
    size_t len = utf8Length(s);
    return len >= width ? s : std::string(width - len, ' ') + s;
}

std::string currentDate() {
    std::time_t t = std::time(nullptr);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%d/%m/%Y %H:%M", std::localtime(&t));
    return buf;
}

} // namespace Utils
