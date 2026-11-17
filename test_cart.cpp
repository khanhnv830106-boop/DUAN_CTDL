// Bộ kiểm thử tự động. Chạy: make test  (có bật AddressSanitizer để bắt rò rỉ/lỗi con trỏ)
#include <iostream>
#include <cstdio>
#include "Cart.h"
#include "Order.h"
#include "FileIO.h"
#include "Utils.h"

static int passed = 0, failed = 0;
#define CHECK(id, cond) do { if (cond) { ++passed; std::cout << "[PASS] " << id << "\n"; } \
    else { ++failed; std::cout << "[FAIL] " << id << "  (" #cond ")\n"; } } while (0)

// Kiểm tra tính toàn vẹn 2 chiều: duyệt xuôi và ngược cho cùng kết quả, đủ count
static bool consistent(const Cart& c) {
    int f = 0, b = 0;
    Node* last = nullptr;
    for (Node* p = c.getHead(); p; p = p->next) {
        if (p->prev != last) return false;
        last = p; ++f;
    }
    if (last != c.getTail()) return false;
    for (Node* p = c.getTail(); p; p = p->prev) ++b;
    return f == c.size() && b == c.size();
}

int main() {
    // ---------- Nhóm A: giỏ rỗng ----------
    { Cart c;
      CHECK("A1 giỏ rỗng: isEmpty, size=0", c.isEmpty() && c.size() == 0);
      CHECK("A2 giỏ rỗng: total=0", c.total() == 0);
      CHECK("A3 giỏ rỗng: xóa ID bất kỳ -> false", !c.removeItem(1));
      CHECK("A4 giỏ rỗng: sửa -> false", !c.updateItem(1, "x", 1, 1));
      CHECK("A5 giỏ rỗng: tìm -> nullptr", c.findById(1) == nullptr);
      c.display(); c.displayReverse(); c.clear();
      CHECK("A6 giỏ rỗng: display/clear không lỗi", consistent(c)); }

    // ---------- Nhóm B: thêm ----------
    { Cart c;
      CHECK("B1 thêm vào giỏ rỗng", c.addItem(1, "Bút", 5000, 2) == Cart::ADDED && c.size() == 1);
      CHECK("B2 head==tail khi 1 phần tử", c.getHead() == c.getTail() && consistent(c));
      CHECK("B3 thêm phần tử thứ 2 vào cuối", c.addItem(2, "Vở", 12000, 1) == Cart::ADDED &&
            c.getTail()->id == 2 && consistent(c));
      CHECK("B4 ID trùng -> cộng dồn SL", c.addItem(1, "Bút", 5000, 3) == Cart::MERGED &&
            c.findById(1)->quantity == 5 && c.size() == 2);
      CHECK("B5 ID trùng giữ nguyên giá cũ", c.addItem(1, "Khác", 999, 1) == Cart::MERGED &&
            c.findById(1)->price == 5000 && c.findById(1)->name == "Bút");
      CHECK("B6 ID <= 0 bị từ chối", c.addItem(0, "A", 1, 1) == Cart::INVALID && c.addItem(-5, "A", 1, 1) == Cart::INVALID);
      CHECK("B7 giá <= 0 bị từ chối", c.addItem(9, "A", 0, 1) == Cart::INVALID && c.addItem(9, "A", -1, 1) == Cart::INVALID);
      CHECK("B8 SL <= 0 bị từ chối", c.addItem(9, "A", 1, 0) == Cart::INVALID && c.addItem(9, "A", 1, -2) == Cart::INVALID);
      CHECK("B9 tên rỗng / có '|' bị từ chối", c.addItem(9, "   ", 1, 1) == Cart::INVALID && c.addItem(9, "a|b", 1, 1) == Cart::INVALID);
      CHECK("B10 chống tràn số lượng", c.addItem(1, "Bút", 5000, 2147483647) == Cart::INVALID);
      CHECK("B11 dữ liệu sai không làm đổi giỏ", c.size() == 2 && consistent(c)); }

    // ---------- Nhóm C: sửa ----------
    { Cart c; c.addItem(1, "Bút", 5000, 2);
      CHECK("C1 sửa đủ 3 trường", c.updateItem(1, "Bút bi", 6000, 4) && c.findById(1)->name == "Bút bi" &&
            c.findById(1)->price == 6000 && c.findById(1)->quantity == 4);
      CHECK("C2 giữ nguyên trường bỏ trống", c.updateItem(1, "", 0, 0) && c.findById(1)->name == "Bút bi" &&
            c.findById(1)->price == 6000 && c.findById(1)->quantity == 4);
      CHECK("C3 chỉ sửa giá", c.updateItem(1, "", 7000, -1) && c.findById(1)->price == 7000 && c.findById(1)->quantity == 4);
      CHECK("C4 sửa ID không tồn tại -> false", !c.updateItem(99, "x", 1, 1)); }

    // ---------- Nhóm D: xóa (4 trường hợp con trỏ) ----------
    { Cart c; for (int i = 1; i <= 5; ++i) c.addItem(i, "H" + std::to_string(i), 1000.0 * i, i);
      CHECK("D1 xóa giữa", c.removeItem(3) && c.size() == 4 && consistent(c) && c.findById(3) == nullptr);
      CHECK("D2 xóa đầu: head đổi", c.removeItem(1) && c.getHead()->id == 2 && c.getHead()->prev == nullptr && consistent(c));
      CHECK("D3 xóa cuối: tail đổi", c.removeItem(5) && c.getTail()->id == 4 && c.getTail()->next == nullptr && consistent(c));
      CHECK("D4 xóa ID không tồn tại", !c.removeItem(77) && c.size() == 2);
      CHECK("D5 xóa phần tử thứ 2/2", c.removeItem(4) && c.size() == 1 && c.getHead() == c.getTail() && consistent(c));
      CHECK("D6 xóa node duy nhất -> head=tail=null", c.removeItem(2) && c.getHead() == nullptr && c.getTail() == nullptr && c.isEmpty());
      CHECK("D7 thêm lại sau khi xóa hết", c.addItem(8, "Mới", 100, 1) == Cart::ADDED && consistent(c)); }

    // ---------- Nhóm E: lõi (copy, insertFront, clear) ----------
    { Cart a; a.addItem(1, "A", 100, 1); a.addItem(2, "B", 200, 2);
      Cart b(a);
      a.removeItem(1);
      CHECK("E1 copy constructor là deep copy", b.size() == 2 && b.findById(1) != nullptr && consistent(b));
      Cart c; c.addItem(7, "Z", 1, 1); c = b;
      b.clear();
      CHECK("E2 operator= là deep copy", c.size() == 2 && consistent(c));
      c = c;
      CHECK("E3 tự gán không lỗi", c.size() == 2 && consistent(c));
      Cart d; d.insertFront(1, "A", 1, 1); d.insertFront(2, "B", 1, 1);
      CHECK("E4 insertFront: head là node mới", d.getHead()->id == 2 && d.getTail()->id == 1 && consistent(d));
      Cart e(std::move(d));
      CHECK("E5 move constructor", e.size() == 2 && d.size() == 0 && d.getHead() == nullptr && consistent(e)); }

    // ---------- Nhóm F: hiển thị / tổng tiền ----------
    { Cart c; c.addItem(1, "A", 1500, 2); c.addItem(2, "B", 2500, 4);
      CHECK("F1 tổng tiền = 1500*2 + 2500*4 = 13000", c.total() == 13000);
      c.display(); c.displayReverse(); }

    // ---------- Nhóm G: sắp xếp ----------
    { Cart c; c.addItem(1, "C", 300, 1); c.addItem(2, "A", 100, 1); c.addItem(3, "B", 200, 1); c.addItem(4, "D", 100, 1);
      c.sortByPrice(true);
      CHECK("G1 sort giá tăng, ổn định (A trước D)", c.getHead()->id == 2 && c.getHead()->next->id == 4 &&
            c.getTail()->id == 1 && consistent(c));
      c.sortByPrice(false);
      CHECK("G2 sort giá giảm", c.getHead()->id == 1 && c.getTail()->price == 100 && consistent(c));
      c.sortByName(true);
      CHECK("G3 sort tên A-Z", c.getHead()->name == "A" && c.getTail()->name == "D" && consistent(c));
      Cart one; one.addItem(1, "X", 1, 1); one.sortByPrice(true);
      Cart none; none.sortByPrice(true);
      CHECK("G4 sort 0 và 1 phần tử", consistent(one) && consistent(none)); }

    // ---------- Nhóm H: đơn hàng ----------
    { Cart c; OrderManager m;
      CHECK("H1 tạo đơn khi giỏ rỗng -> nullptr", m.createOrder(c) == nullptr && m.all().empty());
      c.addItem(1, "A", 1000, 2); c.addItem(2, "B", 500, 3);
      const Order* o = m.createOrder(c);
      CHECK("H2 tạo đơn: tổng đúng, giỏ rỗng", o && o->total == 3500 && o->orderId == 1 && c.isEmpty());
      CHECK("H3 đơn giữ dữ liệu sau khi giỏ bị xóa", m.all()[0].items.size() == 2 && consistent(m.all()[0].items));
      c.addItem(3, "C", 100, 1); m.createOrder(c);
      CHECK("H4 mã đơn tăng dần, 2 đơn", m.all().size() == 2 && m.all()[1].orderId == 2 && m.all()[0].items.size() == 2);
      CHECK("H5 tìm đơn theo mã", m.findOrder(2) != nullptr && m.findOrder(9) == nullptr); }

    // ---------- Nhóm I: file ----------
    { Cart c; c.addItem(1, "Áo thun", 150000.5, 2); c.addItem(2, "Quần", 300000, 1);
      CHECK("I1 lưu giỏ", FileIO::saveCart(c, "t_cart.tmp"));
      Cart d; int skipped = -1;
      CHECK("I2 nạp giỏ khớp dữ liệu", FileIO::loadCart(d, "t_cart.tmp", &skipped) == 2 && skipped == 0 &&
            d.findById(1)->name == "Áo thun" && d.findById(1)->price == 150000.5 && consistent(d));
      Cart e; e.addItem(5, "Giữ", 1, 1);
      CHECK("I3 file không tồn tại -> -1, giỏ giữ nguyên", FileIO::loadCart(e, "khong_co.tmp") == -1 && e.size() == 1);
      { std::FILE* f = std::fopen("t_bad.tmp", "w"); std::fputs("1|A|100|2\nrac roi\n2|B|abc|1\n3|C|50|1\n", f); std::fclose(f); }
      Cart g;
      CHECK("I4 file có dòng lỗi: bỏ qua dòng sai", FileIO::loadCart(g, "t_bad.tmp", &skipped) == 2 && skipped == 2 && g.size() == 2);
      OrderManager m; Cart k; k.addItem(1, "A", 10, 2); m.createOrder(k); k.addItem(2, "B", 20, 1); k.addItem(3, "C", 5, 5); m.createOrder(k);
      CHECK("I5 lưu đơn hàng", FileIO::saveOrders(m, "t_ord.tmp"));
      OrderManager m2;
      CHECK("I6 nạp đơn: đủ số đơn + dữ liệu", FileIO::loadOrders(m2, "t_ord.tmp") == 2 && m2.all()[1].items.size() == 2 &&
            m2.all()[1].total == 45);
      Cart k2; k2.addItem(1, "A", 1, 1);
      CHECK("I7 mã đơn tiếp theo sau khi nạp = 3", m2.createOrder(k2)->orderId == 3);
      { std::FILE* f = std::fopen("t_bad2.tmp", "w"); std::fputs("ORDER|1|d|10|3\n1|A|1|1\n", f); std::fclose(f); }
      OrderManager m3;
      CHECK("I8 file đơn sai định dạng -> -2, không đổi", FileIO::loadOrders(m3, "t_bad2.tmp") == -2 && m3.all().empty());
      std::remove("t_cart.tmp"); std::remove("t_bad.tmp"); std::remove("t_ord.tmp"); std::remove("t_bad2.tmp"); }

    // ---------- Nhóm J: tiện ích ----------
    { int i; double d;
      CHECK("J1 parseInt hợp lệ", Utils::parseInt("42", i) && i == 42 && Utils::parseInt("-7", i) && i == -7);
      CHECK("J2 parseInt từ chối chữ/rỗng/số thực", !Utils::parseInt("abc", i) && !Utils::parseInt("", i) &&
            !Utils::parseInt("12abc", i) && !Utils::parseInt("1.5", i));
      CHECK("J3 parseInt từ chối tràn số", !Utils::parseInt("99999999999", i));
      CHECK("J4 parseDouble hợp lệ / sai", Utils::parseDouble("1500.75", d) && d == 1500.75 && !Utils::parseDouble("x", d) && !Utils::parseDouble("nan", d));
      CHECK("J5 formatMoney", Utils::formatMoney(1234567) == "1,234,567 d" && Utils::formatMoney(0) == "0 d" && Utils::formatMoney(999) == "999 d");
      CHECK("J6 padRight tính ký tự UTF-8", Utils::padRight("Áo", 5) == "Áo   "); }

    std::cout << "\n===== KẾT QUẢ: " << passed << " đạt, " << failed << " lỗi =====\n";
    return failed == 0 ? 0 : 1;
}
