#include "Order.h"
#include "Utils.h"
#include <iostream>
#include <algorithm>
#include <utility>

const Order* OrderManager::createOrder(Cart& cart) {
    if (cart.isEmpty()) return nullptr;
    Order o;
    o.orderId = nextId++;
    o.date    = Utils::currentDate();
    o.items   = cart;              // deep copy qua copy-assignment của Cart
    o.total   = cart.total();
    orders.push_back(std::move(o));
    cart.clear();
    return &orders.back();
}

const Order* OrderManager::findOrder(int orderId) const {
    for (const Order& o : orders) if (o.orderId == orderId) return &o;
    return nullptr;
}

void OrderManager::display() const {
    using namespace Utils;
    if (orders.empty()) { std::cout << "  (Chưa có đơn hàng nào)\n"; return; }
    std::cout << padLeft("Mã đơn", 7) << " | " << padRight("Ngày tạo", 17) << " | "
              << padLeft("Số dòng", 8) << " | " << padLeft("Tổng tiền", 18) << "\n";
    std::cout << std::string(7 + 3 + 17 + 3 + 8 + 3 + 18, '-') << "\n";
    for (const Order& o : orders)
        std::cout << padLeft(std::to_string(o.orderId), 7) << " | " << padRight(o.date, 17) << " | "
                  << padLeft(std::to_string(o.items.size()), 8) << " | "
                  << padLeft(formatMoney(o.total), 18) << "\n";
}

void OrderManager::displayDetail(const Order& o) const {
    std::cout << "\nĐƠN HÀNG #" << o.orderId << "  -  " << o.date << "\n";
    o.items.display();
}

void OrderManager::restore(std::vector<Order>&& loaded) {
    orders = std::move(loaded);
    int mx = 0;
    for (const Order& o : orders) mx = std::max(mx, o.orderId);
    nextId = mx + 1;
}
