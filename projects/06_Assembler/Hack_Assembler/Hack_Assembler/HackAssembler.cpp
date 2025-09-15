#include "HackAssembler.h"


HackAssembler::HackAssembler()
	:code(Code::getInstance())
{
	try {
		std::cout << "prompt>> ";
		std::cin >> filename;

		p = new Parser(filename);

		auto dot_pos = filename.rfind('.');
		auto hack_file = filename.substr(0, dot_pos) + ".hack";

		ofs.open(hack_file);

		Assemble();
	}
	catch (std::exception& e) {
		std::cerr << e.what();
	}
}

void HackAssembler::Assemble()
{
	while (p->advance()) {
		auto insType = p->getInstructionType();

		if (insType == instructionType::A_INSTRUCTION) {
			auto symbol = p->symbol();
			std::bitset<16> binaryRepresentation(stoi(symbol));
			ofs << binaryRepresentation << '\n';
		}
		else if (insType == instructionType::C_INSTRUCTION) {
			auto destStr = p->dest();
			auto compStr = p->comp();
			auto jumpStr = p->jump();

			ofs << "111" + code.comp(compStr) + code.dest(destStr) + code.jump(jumpStr) << '\n';
		}
	}
}

HackAssembler::~HackAssembler()
{
	delete p;
}
