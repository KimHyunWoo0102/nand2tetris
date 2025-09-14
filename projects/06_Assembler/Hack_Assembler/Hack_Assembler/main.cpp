#include"Parser.h"
#include<iostream>

int main() {
	try {
		Parser p("../test.txt");

		std::string str("  add=10 //asldk");
		p.advance(str);
		std::cout << str;
	}
	catch (std::exception& e) {
		std::cerr << e.what();
	}
}