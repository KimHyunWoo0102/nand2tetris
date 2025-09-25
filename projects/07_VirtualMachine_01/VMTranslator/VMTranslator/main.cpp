#include"Parser.h"
#include"CodeWriter.h"

int main() {
	try {
		std::string filename;

		std::cin >> filename;

		CodeWriter cw(filename);
	}
	catch (std::exception& e) {
		std::cerr << e.what();
	}
	return 0;
}