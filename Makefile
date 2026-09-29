CXX ?= c++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic -Werror

.PHONY: all test clean
all: zainful

zainful: main.cpp ZainfulClient.cpp ZainfulClient.h
	$(CXX) $(CXXFLAGS) main.cpp ZainfulClient.cpp -o $@

test: zainful-tests
	./zainful-tests

zainful-tests: tests.cpp ZainfulClient.cpp ZainfulClient.h
	$(CXX) $(CXXFLAGS) tests.cpp ZainfulClient.cpp -o $@

clean:
	rm -f zainful zainful-tests
