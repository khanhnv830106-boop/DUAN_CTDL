#include "FileIO.h"
#include "Utils.h"
#include <fstream>
#include <iomanip>
#include <vector>

namespace {
void writeItem(std::ostream& os, const Node* p) {
    os << p->id << '|' << p->name << '|' << std::fixed << std::setprecision(2)
       << p->price << '|' << p->quantity << '\n';
}
// Đọc 1 dòng "id|tên|giá|sl" vào cart. true nếu hợp lệ.
bool parseItemLine(const std::string& line, Cart& cart) {
    std::vector<std::string> f = Utils::split(line, '|');
    if (f.size() != 4) return false;
    int id, qty; double price;
    if (!Utils::parseInt(Utils::trim(f[0]), id) || !Utils::parseDouble(Utils::trim(f[2]), price) ||
        !Utils::parseInt(Utils::trim(f[3]), qty)) return false;
    return cart.addItem(id, f[1], price, qty) != Cart::INVALID;
}
}

namespace FileIO {

bool saveCart(const Cart& cart, const std::string& path) {
    std::ofstream out(path);
    if (!out) return false;
    for (Node* p = cart.getHead(); p; p = p->next) writeItem(out, p);
    return static_cast<bool>(out);
}

int loadCart(Cart& cart, const std::string& path, int* skipped) {
    std::ifstream in(path);
    if (!in) return -1;
    Cart tmp;
    int ok = 0, bad = 0;
    std::string line;
    while (std::getline(in, line)) {
        if (Utils::trim(line).empty()) continue;
        if (parseItemLine(line, tmp)) ++ok; else ++bad;
    }
    cart = std::move(tmp);
    if (skipped) *skipped = bad;
    return ok;
}

bool saveOrders(const OrderManager& mgr, const std::string& path) {
    std::ofstream out(path);
    if (!out) return false;
    for (const Order& o : mgr.all()) {
        out << "ORDER|" << o.orderId << '|' << o.date << '|' << std::fixed << std::setprecision(2)
            << o.total << '|' << o.items.size() << '\n';
        for (Node* p = o.items.getHead(); p; p = p->next) writeItem(out, p);
    }
    return static_cast<bool>(out);
}

int loadOrders(OrderManager& mgr, const std::string& path) {
    std::ifstream in(path);
    if (!in) return -1;
    std::vector<Order> loaded;
    std::string line;
    while (std::getline(in, line)) {
        if (Utils::trim(line).empty()) continue;
        std::vector<std::string> h = Utils::split(line, '|');
        int id, n; double total;
        if (h.size() != 5 || h[0] != "ORDER" || !Utils::parseInt(h[1], id) ||
            !Utils::parseDouble(h[3], total) || !Utils::parseInt(h[4], n) || n < 0) return -2;
        Order o;
        o.orderId = id; o.date = h[2]; o.total = total;
        for (int i = 0; i < n; ++i) {
            if (!std::getline(in, line) || !parseItemLine(line, o.items)) return -2;
        }
        loaded.push_back(std::move(o));
    }
    int cnt = static_cast<int>(loaded.size());
    mgr.restore(std::move(loaded));
    return cnt;
}

} // namespace FileIO
