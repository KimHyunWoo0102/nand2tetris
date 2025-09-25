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

	class Parser {

		//----------------------------

		enum class CMD_TYPE {
			C_ARITHMETIC, C_PUSH, C_POP, C_LABEL
			, C_GOTO, C_IF, C_FUNCTION, C_RETURN, C_CALL
		};
		
		//----------------------------

		std::string current_command;
		std::ifstream ist;

		//----------------------------

	public:

		//----------------------------
		
		Parser(std::string& filename);

		//----------------------------
		//helper method
		//----------------------------

		bool hasMoreLines();
		void advance();
		std::string getCurrentCmd() const { return current_command; }
		CMD_TYPE commandType() const;

		//----------------------------

		std::string arg1() const;
		int arg2() const;
	};
}