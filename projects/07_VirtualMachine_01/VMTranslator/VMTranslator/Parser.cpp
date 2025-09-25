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
//constructor
//---------------------------------------------------------------


VMParser::Parser::Parser(std::string& filename)
	:ist(filename)
{
	if (!ist.is_open()) {
		throw std::runtime_error("Error : can't find file " + filename);
	}
}

//---------------------------------------------------------------
//private helper
//---------------------------------------------------------------

VMParser::Parser::CMD_TYPE VMParser::Parser::getCommandType() const
{
	std::string first_word;
	auto space_pos = current_command.find(' ');

	if (space_pos == std::string::npos) {
		first_word = current_command;
	}
	else {
		first_word = current_command.substr(0, space_pos);
	}

	if (ARITHMETIC_COMMANDS.count(first_word)) {
		return CMD_TYPE::C_ARITHMETIC;
	}

	if (first_word == "push") {
		return CMD_TYPE::C_PUSH;
	}
	if (first_word == "pop") {
		return CMD_TYPE::C_POP;
	}
	if (first_word == "label") {
		return CMD_TYPE::C_LABEL;
	}
	if (first_word == "if-goto") {
		return CMD_TYPE::C_IF;
	}
	if (first_word == "goto") {
		return CMD_TYPE::C_GOTO;
	}
	if (first_word == "function") {
		return CMD_TYPE::C_FUNCTION;
	}
	if (first_word == "call") {
		return CMD_TYPE::C_CALL;
	}
	if (first_word == "return") {
		return CMD_TYPE::C_RETURN;
	}

	//invalid VM code
	throw std::runtime_error("알 수 없는 명령어입니다: " + first_word);
}

//---------------------------------------------------------------
//helper method
//---------------------------------------------------------------

bool VMParser::Parser::hasMoreLines()
{
	return ist.peek() != EOF;
}

//---------------------------------------------------------------
//advance remove the space
//advance module must be used when hasMoreLines() return true
//---------------------------------------------------------------


void VMParser::Parser::advance()
{
	while (std::getline(ist, current_command)) {
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

		auto cmd_type = getCommandType();

		if (cmd_type == CMD_TYPE::C_ARITHMETIC) {
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
