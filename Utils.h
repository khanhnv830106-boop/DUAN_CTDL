#pragma once
#include <string>
#include <vector>

namespace Utils {
// ---- Phân tích chuỗi (không dùng cin >> nên không bị lỗi bộ đệm) ----
bool parseInt(const std::string& s, int& out);
bool parseDouble(const std::string& s, double& out);

// ---- Nhập liệu an toàn ----
std::string readLine(const std::string& prompt);                  // EOF -> thoát chương trình
int    readInt(const std::string& prompt, int minVal, int maxVal);
double readDouble(const std::string& prompt, double minExclusive);
// Nhập tùy chọn: Enter để bỏ qua (trả về false), nhập sai thì hỏi lại
bool readOptionalInt(const std::string& prompt, int minVal, int maxVal, int& out);
bool readOptionalDouble(const std::string& prompt, double minExclusive, double& out);
bool confirm(const std::string& prompt);

// ---- Kiểm tra / chuỗi ----
bool isValidName(const std::string& s);   // không rỗng, không chứa '|' hoặc xuống dòng
std::string trim(const std::string& s);
std::vector<std::string> split(const std::string& s, char delim);

// ---- Định dạng hiển thị ----
std::string formatMoney(double v);                         // 1234567 -> "1,234,567 d"
std::string padRight(const std::string& s, size_t width);  // tính theo ký tự UTF-8
std::string padLeft(const std::string& s, size_t width);
std::string currentDate();                                 // dd/mm/yyyy hh:mm
}
