//CodeWriter.h
#pragma once

//-------------------------------------------------
//-- INCLUDES --
//-------------------------------------------------

#include<iostream>
#include<fstream>
#include"Parser.h"

//-------------------------------------------------

class CodeWriter {
	std::ofstream ofs;

public:
	CodeWriter(std::string& filename);
	~CodeWriter();

	void writeArithmetic(std::string& command);
	void writePushPop(VMParser::CMD_TYPE command, std::string segment, int index);

};
