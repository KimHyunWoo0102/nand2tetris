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

class HackAssembler {
	std::string filename;
	Parser* p;
	std::ofstream ofs;

public:
	HackAssembler();
	void Assemble();
	~HackAssembler();
};