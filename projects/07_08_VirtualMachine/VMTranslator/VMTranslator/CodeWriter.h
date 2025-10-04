//CodeWriter.h
#pragma once

//-------------------------------------------------
//-- INCLUDES --
//-------------------------------------------------

#include<iostream>
#include<fstream>
#include<string>
#include<map>
#include<functional>

#include"Parser.h"

//-------------------------------------------------

class CodeWriter {
	using ArithmeticGenerator = std::function<std::string()>;

	// ��ɾ�(string)�� ASM ���� �Լ��� �����ϴ� ��
	std::map<std::string, ArithmeticGenerator> arithmeticMap;

	std::ofstream ofs;
	size_t label_counter = 0;

	std::string currentFileName;

	//write Arithmetic helper method
	//���� ������
	std::string makeAddASMCode();
	std::string makeSubASMCode();
	std::string makeAndASMCode();
	std::string makeOrASMCode();

	//���� ������
	std::string makeNegASMCode();
	std::string makeNotASMCode();
	
	//�� ������
	std::string makeEqASMCode();
	std::string makeGTASMCode();
	std::string makeLTASMCode();
	
	//write push pop helper method
	std::string makePushASMCode(const std::string& segment, int index);
	std::string makePopASMCode(const std::string& segment, int index);

	std::string popStackToD();
	std::string pushDToStack();

public:
	CodeWriter();
	~CodeWriter();

	void writeInit();
	void setFileName(const std::string& vmFilename);
	void writeArithmetic(const std::string& command);
	void writePushPop(VMParser::CMD_TYPE command,const std::string& segment, int index);

};
