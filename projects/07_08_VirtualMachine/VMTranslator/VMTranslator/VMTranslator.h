#pragma once

#include"Parser.h"
#include"CodeWriter.h"
#include<string>
#include<vector>

class VMTranslator {
	std::string m_inputPath;
	CodeWriter m_codeWriter;
	std::vector<std::string> m_vmFiles;

public:
	VMTranslator(const std::string& inputPath);
	void translate();
};