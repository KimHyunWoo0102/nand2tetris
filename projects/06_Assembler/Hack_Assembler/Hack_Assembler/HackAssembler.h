//HackAssembler.h
//-------------------------------------------------

#pragma once

//-------------------------------------------------
//includes
//-------------------------------------------------

#include"Code.h"
#include"Parser.h"
#include<string>
#include<algorithm>
#include<iostream>
#include<exception>
#include<bitset>
#include<map>

//---------------------------------------------------------------------
//mnemonic structure
//---------------------------------------------------------------------

struct mnemonic {
	std::map < std::string, int> table;

	void addEntry(std::string symbol, int address);
	bool contain(std::string symbol);
	int getAddress(std::string symbol);

	mnemonic() {
		table["R0"] = 0;
		table["R1"] = 1;
		table["R2"] = 2;
		table["R3"] = 3;
		table["R4"] = 4;
		table["R5"] = 5;
		table["R6"] = 6;
		table["R7"] = 7;
		table["R8"] = 8;
		table["R9"] = 9;
		table["R10"] = 10;
		table["R11"] = 11;
		table["R12"] = 12;
		table["R13"] = 13;
		table["R14"] = 14;
		table["R15"] = 15;

		table["SP"] = 0;
		table["LCL"] = 1;
		table["ARG"] = 2;
		table["THIS"] = 3;
		table["THAT"] = 4;

		table["SCREEN"] = 16384;
		table["KBD"] = 24576;
	}
};

//---------------------------------------------------------------------
//class HackAssembler
//---------------------------------------------------------------------

class HackAssembler {
	std::string filename;
	Parser* p;
	Code& code;
	std::ofstream ofs;
	mnemonic m;

public:
	HackAssembler();

	void Assemble();
	void mapping();

	~HackAssembler();
};