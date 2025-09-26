//CodeWriter.h
#pragma once

//-------------------------------------------------
//-- INCLUDES --
//-------------------------------------------------

#include<iostream>
#include<fstream>
#include"Parser.h"
#include<string>

//-------------------------------------------------

class CodeWriter {
	std::ofstream ofs;

	//helper method
	std::string makePushASMCode(VMParser::CMD_TYPE command, std::string segment, int index);

public:
	CodeWriter(std::string& filename);
	~CodeWriter();

	void writeArithmetic(std::string& command);
	void writePushPop(VMParser::CMD_TYPE command, std::string segment, int index);

};
