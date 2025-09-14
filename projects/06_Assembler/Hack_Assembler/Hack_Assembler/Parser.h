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
	Parser(std::string& filename);
	Parser(const char* filename);
	//------------------------------------------
	// 
	//bool hasMoreLines(); std::getline���� ����
	std::string getCurrentCommend() const { return currentCmd; }
	bool advance();

	instructionType getInstructionType();

	std::string symbol();
	std::string dest();
	std::string comp();
	std::string jump();
};