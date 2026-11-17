// Đo hiệu năng các thao tác của giỏ hàng theo kích thước n. Chạy: make bench
#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>
#include "Cart.h"
using namespace std;
using Clock = chrono::steady_clock;
static double ms(Clock::time_point a, Clock::time_point b) { return chrono::duration<double, milli>(b - a).count(); }

int main() {
    cout << fixed << setprecision(3);
    cout << "== 1. Dựng giỏ n phần tử ==\n";
    cout << setw(9) << "n" << setw(20) << "insertBack (ms)" << setw(24) << "addItem tìm trùng (ms)" << "\n";
    for (int n : {1000, 5000, 10000, 20000}) {
        Cart a; auto t0 = Clock::now();
        for (int i = 1; i <= n; ++i) a.insertBack(i, "SP", 1000, 1);
        auto t1 = Clock::now();
        Cart b; auto t2 = Clock::now();
        for (int i = 1; i <= n; ++i) b.addItem(i, "SP", 1000, 1);
        auto t3 = Clock::now();
        cout << setw(9) << n << setw(20) << ms(t0, t1) << setw(24) << ms(t2, t3) << "\n";
    }
    cout << "\n== 2. Từng thao tác đơn lẻ (trung bình 200 lần lặp, ms) ==\n";
    cout << setw(9) << "n" << setw(14) << "insertBack" << setw(14) << "findById" << setw(16) << "removeNode"
         << setw(16) << "removeItem" << setw(12) << "total" << setw(14) << "sort giá" << setw(12) << "copy" << "\n";
    const int R = 200;
    for (int n : {1000, 10000, 100000, 1000000}) {
        Cart c; for (int i = 1; i <= n; ++i) c.insertBack(i, "SP", 1000.0 + (i * 7919) % 1000, 1);
        auto t0 = Clock::now();
        for (int i = 0; i < R; ++i) c.insertBack(n + 1 + i, "X", 1, 1);
        auto t1 = Clock::now();
        volatile Node* sink = nullptr;
        for (int i = 0; i < R; ++i) sink = c.findById(n);                    // gần cuối: xấu nhất
        auto t2 = Clock::now();
        for (int i = 0; i < R; ++i) c.removeNode(c.getTail());               // O(1)
        auto t3 = Clock::now();
        for (int i = 0; i < R; ++i) { c.insertBack(n + 1 + i, "X", 1, 1); c.removeItem(n + 1 + i); } // find + gỡ
        auto t4 = Clock::now();
        volatile double s = 0;
        for (int i = 0; i < R; ++i) s = c.total();
        auto t5 = Clock::now();
        (void)sink; (void)s;
        Cart d(c);
        auto t6 = Clock::now();
        c.sortByPrice(true);
        auto t7 = Clock::now();
        cout << setw(9) << n << setw(14) << ms(t0, t1) / R << setw(14) << ms(t1, t2) / R
             << setw(16) << ms(t2, t3) / R << setw(16) << ms(t3, t4) / R << setw(12) << ms(t4, t5) / R
             << setw(14) << ms(t6, t7) << setw(12) << ms(t5, t6) << "\n";
    }
    cout << "\n(sort giá và copy: đo 1 lần cho toàn bộ giỏ)\n";
}
