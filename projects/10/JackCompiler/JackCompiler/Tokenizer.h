#pragma once

#include"Token.h"

#include<string>
#include<fstream>
#include<iostream>
#include<sstream>

class Tokenizer {
	std::string currentToken;
	std::stringstream ss;

	std::string removeComments(const std::string& code);
public:
	Tokenizer(const std::string& filename);

	void print() {
		std::cout << ss.str();
	}
};