// QUẢN LÝ GIỎ HÀNG ONLINE - Danh sách liên kết đôi
#include <iostream>
#include <climits>
#include <fstream>
#include "Cart.h"
#include "Order.h"
#include "FileIO.h"
#include "Utils.h"

using namespace std;
using namespace Utils;

static const char* CART_FILE   = "cart.txt";
static const char* ORDERS_FILE = "orders.txt";
static const int   MAX_QTY     = 1000000;

static void addItemUI(Cart& cart) {
    cout << "\n--- THÊM MẶT HÀNG ---\n";
    int id = readInt("ID (số nguyên > 0): ", 1, INT_MAX);
    Node* ex = cart.findById(id);
    if (ex) {
        cout << "ID đã có trong giỏ: " << ex->name << " (" << formatMoney(ex->price)
             << ", SL hiện tại " << ex->quantity << "). Số lượng sẽ được cộng dồn.\n";
        int q = readInt("Số lượng thêm: ", 1, MAX_QTY);
        if (cart.addItem(id, ex->name, ex->price, q) == Cart::MERGED)
            cout << "=> Đã cộng dồn. SL mới: " << cart.findById(id)->quantity << "\n";
        else cout << "=> Không thể cộng dồn (vượt giới hạn).\n";
        return;
    }
    string name;
    while (true) {
        name = readLine("Tên hàng: ");
        if (isValidName(name)) break;
        cout << "  ! Tên không được rỗng và không chứa ký tự '|'.\n";
    }
    double price = readDouble("Đơn giá (> 0): ", 0);
    int qty = readInt("Số lượng: ", 1, MAX_QTY);
    if (cart.addItem(id, name, price, qty) == Cart::ADDED) cout << "=> Đã thêm vào giỏ.\n";
    else cout << "=> Dữ liệu không hợp lệ.\n";
}

static void updateItemUI(Cart& cart) {
    cout << "\n--- SỬA MẶT HÀNG ---\n";
    if (cart.isEmpty()) { cout << "Giỏ hàng trống.\n"; return; }
    int id = readInt("ID cần sửa: ", 1, INT_MAX);
    Node* n = cart.findById(id);
    if (!n) { cout << "=> Không tìm thấy ID " << id << ".\n"; return; }
    cout << "Hiện tại: " << n->name << " | " << formatMoney(n->price) << " | SL " << n->quantity << "\n";
    cout << "(Nhấn Enter để giữ nguyên)\n";
    string name = trim(readLine("Tên mới: "));
    while (!name.empty() && !isValidName(name)) {
        cout << "  ! Tên không được chứa '|'.\n";
        name = trim(readLine("Tên mới: "));
    }
    double price = 0; int qty = 0;
    readOptionalDouble("Đơn giá mới: ", 0, price);
    readOptionalInt("Số lượng mới: ", 1, MAX_QTY, qty);
    cart.updateItem(id, name, price, qty);
    cout << "=> Đã cập nhật.\n";
}

static void removeItemUI(Cart& cart) {
    cout << "\n--- XÓA MẶT HÀNG ---\n";
    if (cart.isEmpty()) { cout << "Giỏ hàng trống.\n"; return; }
    int id = readInt("ID cần xóa: ", 1, INT_MAX);
    Node* n = cart.findById(id);
    if (!n) { cout << "=> Không tìm thấy ID " << id << ".\n"; return; }
    if (confirm("Xóa '" + n->name + "'?")) {
        cart.removeNode(n);
        cout << "=> Đã xóa.\n";
    } else cout << "=> Đã hủy.\n";
}

static void createOrderUI(Cart& cart, OrderManager& mgr) {
    cout << "\n--- TẠO ĐƠN HÀNG ---\n";
    if (cart.isEmpty()) { cout << "Giỏ hàng trống, không thể tạo đơn.\n"; return; }
    cart.display();
    if (!confirm("Xác nhận tạo đơn từ giỏ hàng này?")) { cout << "=> Đã hủy.\n"; return; }
    const Order* o = mgr.createOrder(cart);
    cout << "=> Tạo đơn thành công!\n";
    mgr.displayDetail(*o);
    cout << "(Giỏ hàng đã được làm rỗng)\n";
}

static void historyUI(const OrderManager& mgr) {
    cout << "\n--- LỊCH SỬ ĐƠN HÀNG ---\n";
    mgr.display();
    if (mgr.all().empty()) return;
    int id = readInt("Nhập mã đơn để xem chi tiết (0 = quay lại): ", 0, INT_MAX);
    if (id == 0) return;
    const Order* o = mgr.findOrder(id);
    if (o) mgr.displayDetail(*o); else cout << "=> Không có đơn #" << id << ".\n";
}

static void sortUI(Cart& cart) {
    cout << "\n--- SẮP XẾP GIỎ HÀNG ---\n";
    if (cart.size() < 2) { cout << "Cần ít nhất 2 mặt hàng để sắp xếp.\n"; return; }
    cout << "1. Giá tăng dần\n2. Giá giảm dần\n3. Tên A-Z\n4. Tên Z-A\n";
    int c = readInt("Chọn: ", 1, 4);
    if (c == 1) cart.sortByPrice(true);
    else if (c == 2) cart.sortByPrice(false);
    else if (c == 3) cart.sortByName(true);
    else cart.sortByName(false);
    cart.display();
}

static void saveAll(const Cart& cart, const OrderManager& mgr) {
    bool a = FileIO::saveCart(cart, CART_FILE);
    bool b = FileIO::saveOrders(mgr, ORDERS_FILE);
    cout << (a && b ? "=> Đã lưu " : "=> Lưu thất bại: ") << CART_FILE << ", " << ORDERS_FILE << "\n";
}

static void loadAll(Cart& cart, OrderManager& mgr, bool quiet) {
    int skipped = 0;
    int a = FileIO::loadCart(cart, CART_FILE, &skipped);
    int b = FileIO::loadOrders(mgr, ORDERS_FILE);
    if (quiet && a == -1 && b == -1) return;
    if (a >= 0) cout << "Đã nạp " << a << " mặt hàng" << (skipped ? " (bỏ qua " + to_string(skipped) + " dòng lỗi)" : "") << ".\n";
    else if (!quiet) cout << "Không đọc được " << CART_FILE << ".\n";
    if (b >= 0) cout << "Đã nạp " << b << " đơn hàng.\n";
    else if (b == -2) cout << "File " << ORDERS_FILE << " sai định dạng, bỏ qua.\n";
    else if (!quiet) cout << "Không đọc được " << ORDERS_FILE << ".\n";
}

static void printMenu(const Cart& cart) {
    cout << "\n========== GIỎ HÀNG ONLINE (" << cart.size() << " mặt hàng) ==========\n"
         << " 1. Thêm mặt hàng         6. Xem lịch sử đơn hàng\n"
         << " 2. Xem giỏ hàng          7. Sắp xếp giỏ hàng\n"
         << " 3. Sửa mặt hàng          8. Xem giỏ hàng theo chiều ngược\n"
         << " 4. Xóa mặt hàng          9. Lưu ra file\n"
         << " 5. Tạo đơn hàng         10. Nạp từ file\n"
         << " 0. Thoát\n";
}

int main() {
    system("chcp 65001 > nul");
    Cart cart;
    OrderManager mgr;
    loadAll(cart, mgr, true);
    while (true) {
        printMenu(cart);
        switch (readInt("Chọn chức năng: ", 0, 10)) {
            case 1: addItemUI(cart); break;
            case 2: cout << "\n--- GIỎ HÀNG ---\n"; cart.display(); break;
            case 3: updateItemUI(cart); break;
            case 4: removeItemUI(cart); break;
            case 5: createOrderUI(cart, mgr); break;
            case 6: historyUI(mgr); break;
            case 7: sortUI(cart); break;
            case 8: cout << "\n--- GIỎ HÀNG (NGƯỢC) ---\n"; cart.displayReverse(); break;
            case 9: saveAll(cart, mgr); break;
            case 10: loadAll(cart, mgr, false); break;
            case 0:
                if (confirm("Lưu dữ liệu trước khi thoát?")) saveAll(cart, mgr);
                cout << "Tạm biệt!\n";
                return 0;
        }
    }
}
