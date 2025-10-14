#include"Tokenizer.h"

int main() {
	std::string filename;
	std::cin >> filename;

	Tokenizer tn(filename);

	tn.print();
	return 0;
}