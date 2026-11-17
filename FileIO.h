#pragma once
#include <string>
#include "Cart.h"
#include "Order.h"

namespace FileIO {
// Định dạng giỏ: mỗi dòng  id|tên|giá|số lượng
// Trả về số dòng hợp lệ đã đọc; -1 nếu không mở được file (giỏ giữ nguyên).
bool saveCart(const Cart& cart, const std::string& path);
int  loadCart(Cart& cart, const std::string& path, int* skipped = nullptr);

// Định dạng đơn: dòng  ORDER|mã|ngày|tổng|n  rồi n dòng mặt hàng.
// Trả về số đơn đọc được; -1 không mở được file; -2 sai định dạng (không thay đổi gì).
bool saveOrders(const OrderManager& mgr, const std::string& path);
int  loadOrders(OrderManager& mgr, const std::string& path);
}
