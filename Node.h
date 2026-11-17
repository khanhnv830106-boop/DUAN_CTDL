#pragma once
#include <string>

// Node của danh sách liên kết đôi: một mặt hàng trong giỏ.
struct Node {
    int id;
    std::string name;
    double price;
    int quantity;
    Node* prev;
    Node* next;

    Node(int id_, const std::string& name_, double price_, int quantity_)
        : id(id_), name(name_), price(price_), quantity(quantity_),
          prev(nullptr), next(nullptr) {}
};
