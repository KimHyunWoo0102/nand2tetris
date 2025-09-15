#include"Parser.h"
#include"Code.h"
#include<iostream>

int main() {
	try {
		auto& code = Code::getInstance();
		std::cout<<code.dest("DM");
	}
	catch (std::exception& e) {
		std::cerr << e.what();
	}
}