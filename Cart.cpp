#include "Cart.h"
#include "Utils.h"
#include <iostream>
#include <climits>
#include <cmath>
#include <utility>

// ===================== Vòng đời =====================
Cart::Cart() : head(nullptr), tail(nullptr), count(0) {}

Cart::Cart(const Cart& other) : head(nullptr), tail(nullptr), count(0) { copyFrom(other); }

Cart& Cart::operator=(const Cart& other) {
    if (this != &other) {
        Cart tmp(other);                    // copy trước, an toàn nếu lỗi
        std::swap(head, tmp.head);
        std::swap(tail, tmp.tail);
        std::swap(count, tmp.count);
    }
    return *this;
}

Cart::Cart(Cart&& other) noexcept : head(other.head), tail(other.tail), count(other.count) {
    other.head = other.tail = nullptr;
    other.count = 0;
}

Cart& Cart::operator=(Cart&& other) noexcept {
    if (this != &other) {
        clear();
        head = other.head; tail = other.tail; count = other.count;
        other.head = other.tail = nullptr;
        other.count = 0;
    }
    return *this;
}

Cart::~Cart() { clear(); }

void Cart::copyFrom(const Cart& other) {
    for (Node* p = other.head; p != nullptr; p = p->next)
        insertBack(p->id, p->name, p->price, p->quantity);
}

// ===================== Thao tác lõi =====================
void Cart::insertFront(int id, const std::string& name, double price, int qty) {
    Node* n = new Node(id, name, price, qty);
    n->next = head;
    if (head) head->prev = n; else tail = n;   // danh sách rỗng: node là cả head lẫn tail
    head = n;
    ++count;
}

void Cart::insertBack(int id, const std::string& name, double price, int qty) {
    Node* n = new Node(id, name, price, qty);
    n->prev = tail;
    if (tail) tail->next = n; else head = n;
    tail = n;
    ++count;
}

Node* Cart::findById(int id) const {
    for (Node* p = head; p != nullptr; p = p->next)
        if (p->id == id) return p;
    return nullptr;
}

void Cart::removeNode(Node* node) {
    if (!node) return;
    if (node->prev) node->prev->next = node->next; else head = node->next;  // xóa đầu
    if (node->next) node->next->prev = node->prev; else tail = node->prev;  // xóa cuối
    delete node;
    --count;
}

void Cart::clear() {
    Node* p = head;
    while (p) {
        Node* nxt = p->next;
        delete p;
        p = nxt;
    }
    head = tail = nullptr;
    count = 0;
}

double Cart::total() const {
    double sum = 0;
    for (Node* p = head; p != nullptr; p = p->next) sum += p->price * p->quantity;
    return sum;
}

// ===================== Chức năng nghiệp vụ =====================
Cart::AddResult Cart::addItem(int id, const std::string& name, double price, int qty) {
    if (id <= 0 || qty <= 0 || !(price > 0) || !std::isfinite(price)) return INVALID;
    Node* ex = findById(id);
    if (ex) {
        if (qty > INT_MAX - ex->quantity) return INVALID;   // chống tràn số
        ex->quantity += qty;
        return MERGED;
    }
    if (!Utils::isValidName(name)) return INVALID;
    insertBack(id, Utils::trim(name), price, qty);
    return ADDED;
}

bool Cart::updateItem(int id, const std::string& newName, double newPrice, int newQty) {
    Node* n = findById(id);
    if (!n) return false;
    if (Utils::isValidName(newName)) n->name = Utils::trim(newName);
    if (newPrice > 0 && std::isfinite(newPrice)) n->price = newPrice;
    if (newQty > 0) n->quantity = newQty;
    return true;
}

bool Cart::removeItem(int id) {
    Node* n = findById(id);
    if (!n) return false;
    removeNode(n);
    return true;
}

// ===================== Hiển thị =====================
void Cart::printRows(bool reverse) const {
    using namespace Utils;
    if (isEmpty()) { std::cout << "  (Giỏ hàng trống)\n"; return; }
    std::cout << padLeft("STT", 4) << " | " << padLeft("ID", 6) << " | " << padRight("Tên hàng", 24)
              << " | " << padLeft("Đơn giá", 16) << " | " << padLeft("SL", 5)
              << " | " << padLeft("Thành tiền", 18) << "\n";
    std::cout << std::string(4 + 3 + 6 + 3 + 24 + 3 + 16 + 3 + 5 + 3 + 18, '-') << "\n";
    int stt = reverse ? count : 1;
    for (Node* p = reverse ? tail : head; p != nullptr; p = reverse ? p->prev : p->next) {
        std::cout << padLeft(std::to_string(stt), 4) << " | " << padLeft(std::to_string(p->id), 6)
                  << " | " << padRight(p->name, 24) << " | " << padLeft(formatMoney(p->price), 16)
                  << " | " << padLeft(std::to_string(p->quantity), 5)
                  << " | " << padLeft(formatMoney(p->price * p->quantity), 18) << "\n";
        stt += reverse ? -1 : 1;
    }
    std::cout << std::string(4 + 3 + 6 + 3 + 24 + 3 + 16 + 3 + 5 + 3 + 18, '-') << "\n";
    std::cout << padLeft("TỔNG CỘNG: " + formatMoney(total()), 84) << "\n";
}

void Cart::display() const        { printRows(false); }
void Cart::displayReverse() const { printRows(true);  }

// ===================== Merge sort trên DSLK đôi =====================
static Node* mergeLists(Node* a, Node* b, bool (*lessEq)(const Node*, const Node*, bool), bool asc) {
    Node dummy(0, "", 0, 0);
    Node* t = &dummy;
    while (a && b) {
        if (lessEq(a, b, asc)) { t->next = a; a = a->next; }   // <= giữ tính ổn định
        else                   { t->next = b; b = b->next; }
        t = t->next;
    }
    t->next = a ? a : b;
    Node* res = dummy.next;
    dummy.next = nullptr;
    return res;
}

static Node* mergeSort(Node* h, bool (*lessEq)(const Node*, const Node*, bool), bool asc) {
    if (!h || !h->next) return h;
    Node* slow = h; Node* fast = h->next;           // chia đôi bằng con trỏ nhanh/chậm
    while (fast && fast->next) { slow = slow->next; fast = fast->next->next; }
    Node* mid = slow->next;
    slow->next = nullptr;
    return mergeLists(mergeSort(h, lessEq, asc), mergeSort(mid, lessEq, asc), lessEq, asc);
}

static bool priceLessEq(const Node* a, const Node* b, bool asc) {
    return asc ? a->price <= b->price : a->price >= b->price;
}
static bool nameLessEq(const Node* a, const Node* b, bool asc) {
    return asc ? a->name <= b->name : a->name >= b->name;
}

void Cart::sortList(bool (*lessEq)(const Node*, const Node*, bool), bool asc) {
    head = mergeSort(head, lessEq, asc);
    Node* p = nullptr;                               // dựng lại prev và tail trong 1 lượt
    for (Node* n = head; n != nullptr; n = n->next) { n->prev = p; p = n; }
    tail = p;
}

void Cart::sortByPrice(bool ascending) { sortList(priceLessEq, ascending); }
void Cart::sortByName(bool ascending)  { sortList(nameLessEq, ascending); }
