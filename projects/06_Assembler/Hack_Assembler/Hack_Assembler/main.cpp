#include"Parser.h"
#include<iostream>

int main() {
	try {
		Parser p("../test.txt");
		while (p.advance()) {

		}
	}
	catch (std::exception& e) {
		std::cerr << e.what();
	}
}