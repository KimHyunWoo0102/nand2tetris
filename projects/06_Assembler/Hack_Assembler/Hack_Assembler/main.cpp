#include"HackAssembler.h"
#include<iostream>

int main() {
	try {
		HackAssembler assembler;
	}
	catch (std::exception& e) {
		std::cerr << e.what();
	}
}