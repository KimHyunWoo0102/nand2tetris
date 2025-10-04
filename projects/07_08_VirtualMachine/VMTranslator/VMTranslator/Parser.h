//Parser.h
//----------------------------
#pragma once
//----------------------------
//-- INCLUDES --
//----------------------------

#include<iostream>
#include<fstream>
#include<string>
#include<vector>

//----------------------------

namespace VMParser {

	enum class CMD_TYPE {
		C_ARITHMETIC, C_PUSH, C_POP, C_LABEL
		, C_GOTO, C_IF, C_FUNCTION, C_RETURN, C_CALL
	};

	class Parser {
		
		//-----------------------------
		//private helper
		//-----------------------------

		CMD_TYPE getCommandType(std::string& command) const;

		//----------------------------

		std::string current_command;
		std::ifstream ifs;
		CMD_TYPE m_commandType;
		std::string m_arg1;
		int m_arg2;

		//----------------------------

	public:

		//----------------------------
		
		Parser(const std::string& filename);
		~Parser();

		//----------------------------
		//helper method
		//----------------------------

		bool hasMoreLines();
		void advance();
		std::string getCurrentCmd() const { return current_command; }

		//----------------------------

		CMD_TYPE commandType()const { return  m_commandType; }
		std::string arg1() const { return m_arg1; }
		int arg2() const { return m_arg2; }
	};
}