#pragma once

#include"Parser.h"
#include"CodeWriter.h"
#include<string>

class VMTranslator {
	std::string m_filename;
	VMParser::Parser m_parser;
	CodeWriter m_codeWriter;

public:
	VMTranslator(const std::string& vm_filename);
	void translate();
};