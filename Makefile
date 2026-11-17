CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
SRC      = Cart.cpp Order.cpp Utils.cpp FileIO.cpp

all: cart

cart: main.cpp $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

test: tests/test_cart.cpp $(SRC)
	$(CXX) $(CXXFLAGS) -I. -fsanitize=address,undefined -g -o test_cart $^
	./test_cart

bench: tests/bench.cpp $(SRC)
	$(CXX) $(CXXFLAGS) -I. -o bench $^
	./bench

clean:
	rm -f cart test_cart bench cart.txt orders.txt *.tmp
