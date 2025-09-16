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
	Parser(const std::string& filename);

	//------------------------------------------

	std::string getCurrentCommend() const { return currentCmd; }
	bool advance();
	void reset() {
		ifs.clear(); // 파일 스트림 상태 초기화
		ifs.seekg(0, std::ios::beg);
	}

	//------------------------------------------

	instructionType getInstructionType() const;
	std::string symbol() const;
	std::string dest() const;
	std::string comp() const;
	std::string jump() const;
};