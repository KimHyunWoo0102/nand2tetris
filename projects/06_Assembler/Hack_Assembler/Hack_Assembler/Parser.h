#pragma once

//--------------------------------------
//Parser.h
//--------------------------------------

//--------------------------------------
//includes
//--------------------------------------

#include<iostream>
#include<fstream>
#include<string>
#include<map>

//--------------------------------------
// ��ɾ� Ÿ�� ��ȯ�ϴ� enum class
//--------------------------------------

enum class instructionType { A_INSTRUCTION, C_INSTRUCTION, L_INSTRUCTION };

//--------------------------------------
// class Parser
//--------------------------------------

class Parser {
	std::ifstream ifs;
	std::string currentCmd;

public:
	Parser(const std::string& filename);

	//------------------------------------------

	std::string getCurrentCommend() const { return currentCmd; }
	bool advance();

	//------------------------------------------

	instructionType getInstructionType() const;
	std::string symbol() const;
	std::string dest() const;
	std::string comp() const;
	std::string jump() const;
};