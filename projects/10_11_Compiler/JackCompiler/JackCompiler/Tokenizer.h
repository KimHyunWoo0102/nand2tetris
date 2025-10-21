#pragma once

#include"Token.h"

#include<string>
#include<fstream>
#include<iostream>
#include<sstream>

class Tokenizer {
	std::stringstream ss;

	Token::Token currentToken;
	Token::Token nextToken;
	
	//private helper method
	std::string removeComments(const std::string& code);
	void skipWhitespace();
	bool isSymbol(char c);
	void readNextToken();

public:
	Tokenizer(const std::string& filename);

	bool hasMoreTokens();
	void advance();

	//getter
	const Token::Token& getCurrentToken()const { return currentToken; }
	const Token::Token& peekToken()const { return nextToken; }

	int intVal() const;
	char symbol() const;
	const std::string& identifier()const;
	const std::string& stringVal()const;
	Token::KeywordType keyword() const;
	Token::TokenType tokenType() const;

	//public helper method
	void print() {
		std::cout << ss.str();
	}
};