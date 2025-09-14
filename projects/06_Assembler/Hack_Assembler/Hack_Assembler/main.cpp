#include"Parser.h"
#include<iostream>

int main() {
	try {
		Parser p("../test.txt");
		while (p.advance()) {
			std::cout << p.symbol() << '\n';
			std::cout << p.dest() << '\n';
			std::cout << p.comp() << '\n';
			std::cout << p.jump() << '\n';
		}
	}
	catch (std::exception& e) {
		std::cerr << e.what();
	}
}