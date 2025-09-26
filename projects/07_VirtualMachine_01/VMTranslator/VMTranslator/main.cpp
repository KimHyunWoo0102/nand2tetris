#include"Parser.h"
#include"CodeWriter.h"

int main() {
	try {
		std::string filename;

		std::cin >> filename;

		VMParser::Parser p(filename);
		p.advance();

		CodeWriter cw(filename);

		cw.writePushPop(p.commandType(), p.arg1(), p.arg2());

	}
	catch (std::exception& e) {
		std::cerr << e.what();
	}
	return 0;
}