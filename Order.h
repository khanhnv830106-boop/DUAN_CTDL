#pragma once
#include <string>
#include <vector>
#include "Cart.h"

struct Order {
    int         orderId = 0;
    std::string date;
    Cart        items;       // bản sao sâu của giỏ tại thời điểm đặt
    double      total = 0;
};

class OrderManager {
private:
    std::vector<Order> orders;
    int nextId;

public:
    OrderManager() : nextId(1) {}

    // Tạo đơn từ giỏ: sao chép sâu, tính tổng, làm rỗng giỏ. nullptr nếu giỏ rỗng.
    const Order* createOrder(Cart& cart);

    const std::vector<Order>& all() const { return orders; }
    const Order* findOrder(int orderId) const;
    void display() const;                       // danh sách tóm tắt
    void displayDetail(const Order& o) const;   // chi tiết 1 đơn
    void restore(std::vector<Order>&& loaded);  // dùng khi đọc file
};
