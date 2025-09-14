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
	bool hasMoreLines();
	std::string getCurrentCommend() const { return currentCmd; }
	void advance(std::string& line);


};