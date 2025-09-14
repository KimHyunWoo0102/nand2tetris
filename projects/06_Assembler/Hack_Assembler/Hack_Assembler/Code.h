//Code.h
//----------------------------------------------
#pragma once

//----------------------------------------------
//includes
//----------------------------------------------

#include<string>
#include<iostream>
#include<map>

//----------------------------------------------

using binary = std::string;
using instruction = std::string;

class Code {
	std::map<instruction, binary> cmp_map;
	std::map<instruction, binary> dest_map;
	std::map<instruction, binary> jump_map;

public:
	Code() = default;

	std::string dest(std::string& input);
	std::string comp(std::string& input);
	std::string jump(std::string& input);
};
