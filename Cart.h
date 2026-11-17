#pragma once
#include <string>
#include "Node.h"

// Giỏ hàng cài đặt bằng danh sách liên kết ĐÔI (có head + tail).
// Tuân thủ Rule of Five để sao chép sâu (deep copy) và không rò rỉ bộ nhớ.
class Cart {
private:
    Node* head;
    Node* tail;
    int   count;

public:
    enum AddResult { ADDED, MERGED, INVALID };

    // ---- Vòng đời (Rule of Five) ----
    Cart();
    Cart(const Cart& other);
    Cart& operator=(const Cart& other);
    Cart(Cart&& other) noexcept;
    Cart& operator=(Cart&& other) noexcept;
    ~Cart();

    // ---- Truy vấn ----
    bool  isEmpty() const { return count == 0; }
    int   size() const { return count; }
    Node* getHead() const { return head; }
    Node* getTail() const { return tail; }
    Node* findById(int id) const;          // O(n)
    double total() const;                  // O(n)

    // ---- Thao tác lõi ----
    void insertFront(int id, const std::string& name, double price, int qty);  // O(1)
    void insertBack (int id, const std::string& name, double price, int qty);  // O(1)
    void removeNode(Node* node);           // O(1): gỡ node đã biết con trỏ
    void clear();                          // O(n)

    // ---- Chức năng nghiệp vụ ----
    // ID chưa có: chèn cuối. ID đã có: cộng dồn số lượng (giữ tên/giá cũ).
    AddResult addItem(int id, const std::string& name, double price, int qty);
    // name rỗng / price<=0 / qty<=0 nghĩa là "giữ nguyên". false nếu không có ID.
    bool updateItem(int id, const std::string& newName, double newPrice, int newQty);
    bool removeItem(int id);               // O(n) tìm + O(1) gỡ

    // ---- Hiển thị / nâng cao ----
    void display() const;                  // duyệt xuôi head -> tail
    void displayReverse() const;           // duyệt ngược tail -> head
    void sortByPrice(bool ascending);      // merge sort O(n log n), ổn định
    void sortByName(bool ascending);

private:
    void copyFrom(const Cart& other);
    void printRows(bool reverse) const;
    void sortList(bool (*lessEq)(const Node*, const Node*, bool), bool asc);
};
