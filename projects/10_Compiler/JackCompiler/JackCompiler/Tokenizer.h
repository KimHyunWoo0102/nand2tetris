#pragma once

#include"Token.h"

#include<string>
#include<fstream>
#include<iostream>
#include<sstream>

class Tokenizer {
	std::string currentToken;
	std::stringstream ss;

	Token::Token token;
	
	//private helper method
	std::string removeComments(const std::string& code);
	void skipWhitespace();
	bool isSymbol(char c);

public:
	Tokenizer(const std::string& filename);

	bool hasMoreTokens();
	void advance();
	Token::Token getCurrentToken() { return token; }

	//public helper method
	void print() {
		std::cout << ss.str();
	}
};