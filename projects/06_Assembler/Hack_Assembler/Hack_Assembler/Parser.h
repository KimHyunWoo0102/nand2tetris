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
// 명령어 타입 반환하는 enum class
//--------------------------------------

enum class instructionType { A_INSTRUCTION, C_INSTRUCTION, L_INSTRUCTION };

//--------------------------------------
// class Parser
//--------------------------------------

class Parser {
	std::ifstream ifs;
	std::string currentCmd;

public:
	Parser(std::string& filename);
	Parser(const char* filename);
	//------------------------------------------
	// 
	//bool hasMoreLines(); std::getline으로 변경
	std::string getCurrentCommend() const { return currentCmd; }
	bool advance();

	instructionType getInstructionType();

	std::string symbol();
	std::string dest();
	std::string comp();
	std::string jump();
};