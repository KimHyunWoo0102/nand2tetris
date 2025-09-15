#include "HackAssembler.h"


HackAssembler::HackAssembler()
{
	try {
		std::cout << "prompt>> ";
		std::cin >> filename;

		p = new Parser(filename);
		
		auto dot_pos = filename.rfind('.');
		auto hack_file = filename.substr(0, dot_pos) + ".txt";

		std::cout << hack_file << '\n';
		ofs.open("tt.txt");

		Assemble();
	}
	catch (std::exception& e) {
		std::cerr << e.what();
	}
}

void HackAssembler::Assemble()
{
	while (p->advance()) {
		auto insType = instructionType();

		if (insType == instructionType::A_INSTRUCTION) {
			auto symbol = p->symbol();
			std::bitset<16> binaryRepresentation(stoi(symbol));
			ofs << binaryRepresentation;
		}
		else if (insType == instructionType::C_INSTRUCTION) {

		}
	}
}

HackAssembler::~HackAssembler()
{
	delete p;
}
