//Parser.cpp
#include "Parser.h"
#include <unordered_set> // 산술 명령어 그룹을 위해 추가
#include <sstream> 

//---------------------------------------------------------------
//ARITHMETIC set for commandType method
//---------------------------------------------------------------

const std::unordered_set<std::string> ARITHMETIC_COMMANDS = {
	"add", "sub", "neg", "eq", "gt", "lt", "and", "or", "not"
};

//---------------------------------------------------------------
//constructor && destuctor
//---------------------------------------------------------------


VMParser::Parser::Parser(std::string& filename)
	:ifs(filename)
{
	if (!ifs.is_open()) {
		throw std::runtime_error("Error : can't find file " + filename);
	}
}

//---------------------------------------------------------------

VMParser::Parser::~Parser()
{
	ifs.close();
}

//---------------------------------------------------------------
//private helper
//---------------------------------------------------------------

VMParser::CMD_TYPE VMParser::Parser::getCommandType(std::string& command) const
{
	if (ARITHMETIC_COMMANDS.count(command)) {
		return CMD_TYPE::C_ARITHMETIC;
	}

	if (command == "push") {
		return CMD_TYPE::C_PUSH;
	}
	if (command == "pop") {
		return CMD_TYPE::C_POP;
	}
	if (command == "label") {
		return CMD_TYPE::C_LABEL;
	}
	if (command == "if-goto") {
		return CMD_TYPE::C_IF;
	}
	if (command == "goto") {
		return CMD_TYPE::C_GOTO;
	}
	if (command == "function") {
		return CMD_TYPE::C_FUNCTION;
	}
	if (command == "call") {
		return CMD_TYPE::C_CALL;
	}
	if (command == "return") {
		return CMD_TYPE::C_RETURN;
	}

	//invalid VM code
	throw std::runtime_error("알 수 없는 명령어입니다: " + command);
}

//---------------------------------------------------------------
//helper method
//---------------------------------------------------------------

bool VMParser::Parser::hasMoreLines()
{
	return ifs.peek() != EOF;
}

//---------------------------------------------------------------
//advance remove the space
//advance module must be used when hasMoreLines() return true
//---------------------------------------------------------------


void VMParser::Parser::advance()
{
	while (std::getline(ifs, current_command)) {
		//remove annotation

		auto annotation_pos = current_command.find("//");

		if (annotation_pos != std::string::npos) {
			current_command = current_command.substr(0, annotation_pos);
		}
		
		//remove space 

		auto first = current_command.find_first_not_of(" \t\n\r\f\v");

		if (first == std::string::npos) {
			continue;
		}//if empty line, then continue

		auto last = current_command.find_last_not_of(" \t\n\r\f\v");

		if (last != std::string::npos) {
			current_command = current_command.substr(first, last - first + 1);
		}

		std::stringstream ss(current_command);

		std::string cmd;
		ss >> cmd;

		m_commandType = getCommandType(cmd);

		if (m_commandType == CMD_TYPE::C_ARITHMETIC) {
			this->m_arg1 = current_command;
		}
		else if (m_commandType == CMD_TYPE::C_PUSH ||
			m_commandType == CMD_TYPE::C_POP ||
			m_commandType == CMD_TYPE::C_FUNCTION ||
			m_commandType == CMD_TYPE::C_CALL) {
			ss >> m_arg1;
			ss >> m_arg2;
		}
		else { // C_LABEL, C_GOTO, C_IF
			ss >> m_arg1;
		}

		if (current_command.size())return;
	}
}

//---------------------------------------------------------------
