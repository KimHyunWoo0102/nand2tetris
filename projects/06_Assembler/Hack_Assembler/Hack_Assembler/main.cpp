#include"Parser.h"
#include<iostream>

int main() {
	try {
		Parser p("../test.txt");
		while (p.advance()) {
			std::cout << p.symbol() << '\n';
		}
	}
	catch (std::exception& e) {
		std::cerr << e.what();
	}
}