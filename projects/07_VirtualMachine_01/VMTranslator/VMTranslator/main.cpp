#include"Parser.h"

int main() {
	try {
		std::string filename;

		std::cin >> filename;

		VMParser::Parser p(filename);

		while (p.hasMoreLines()) {
			p.advance();

			std::cout << p.getCurrentCmd() << "\n";
			std::cout << p.arg1() << '\n';
			std::cout << p.arg2() << '\n';
		}
	}
	catch (std::exception& e) {
		std::cerr << e.what();
	}
	return 0;
}