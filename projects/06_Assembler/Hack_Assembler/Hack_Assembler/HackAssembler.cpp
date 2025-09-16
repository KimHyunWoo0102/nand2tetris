//HackAssembler.cpp
//----------------------------------------------------------------

#include "HackAssembler.h"

//----------------------------------------------------------------
//R0~R15 저장이후 다음에 저장할 주소
//----------------------------------------------------------------

int nextAvailableRAM = 16;

//----------------------------------------------------------------
//mapping 할 mnemonic의 helper함수
//----------------------------------------------------------------

void mnemonic::addEntry(std::string symbol, int address)
{
	table[symbol] = address;
}

//----------------------------------------------------------------

bool mnemonic::contain(std::string symbol)
{
	return table.find(symbol) != table.end();
}

//----------------------------------------------------------------

int mnemonic::getAddress(std::string symbol)
{
	if (contain(symbol)) {
		return table[symbol];
	}

	return -1;
}

//----------------------------------------------------------------
//HackAssembler constructor && destructor
//----------------------------------------------------------------

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

		//2-way assemble
		mapping();
		Assemble();
	}
	catch (std::exception& e) {
		std::cerr << "Error: " << e.what() << '\n';
		// 예외 발생 시 자원 해제
		if (p) delete p;
		if (ofs.is_open()) ofs.close();
	}
}

//----------------------------------------------------------------

HackAssembler::~HackAssembler()
{
	delete p;
}

//----------------------------------------------------------------

void HackAssembler::mapping()
{
	p->reset();

	int pc = 0;

	while (p->advance()) {
		auto insType = p->getInstructionType();

		if (insType == instructionType::L_INSTRUCTION) {
			auto symbol = p->symbol();

			if (!m.contain(symbol)) {
				m.addEntry(symbol, pc);
			}
		}
		else
		{
			pc++;
		}
	}
}

//----------------------------------------------------------------

void HackAssembler::Assemble()
{
	p->reset();

	while (p->advance()) {
		auto insType = p->getInstructionType();

		if (insType == instructionType::A_INSTRUCTION) {
			auto symbol = p->symbol();
			std::bitset<16> binaryRepresentation;

			// 1. 숫자인지 확인
			if (isdigit(symbol[0])) {
				binaryRepresentation = std::stoi(symbol);
			}
			// 2. 심볼 테이블에 있는지 확인 (레이블 또는 미리 정의된 심볼)
			else if (m.contain(symbol)) {
				binaryRepresentation = m.getAddress(symbol);
			}
			// 3. 처음 등장하는 변수 심볼이면 RAM 16번지부터 할당
			else {
				m.addEntry(symbol, nextAvailableRAM);
				binaryRepresentation = nextAvailableRAM;
				nextAvailableRAM++;
			}
			ofs << binaryRepresentation.to_string() << '\n';
		}
		else if (insType == instructionType::C_INSTRUCTION) {
			auto destStr = p->dest();
			auto compStr = p->comp();
			auto jumpStr = p->jump();

			ofs << "111" + code.comp(compStr) + code.dest(destStr) + code.jump(jumpStr) << '\n';
		}
	}
}



