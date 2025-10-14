#include "Tokenizer.h"

//----------------------------------------------------------------
// private helper method
//----------------------------------------------------------------

std::string Tokenizer::removeComments(const std::string& code)
{
	std::string cleanCode;
	bool inMultiLineComment = false;
	bool inLineComment = false;

	size_t codeLength = code.length();

	for (size_t i = 0; i < codeLength; i++) {
		if (inLineComment) {
			if (code[i] == '\n') {
				inLineComment = false;
				cleanCode += '\n';
			}
		}
		else if (inMultiLineComment) {
			if (code[i] == '*' && (i + 1) < codeLength && code[i + 1] == '/') {
				inMultiLineComment = false;
				i++;
			}
		}
		else {
			if (code[i] == '/'&&(i+1)<codeLength) {
				if (code[i + 1] == '/') {
					inLineComment = true;
					++i;
				}
				else if (code[i + 1] == '*') {
					inMultiLineComment = true;
					++i;
				}
				else {
					cleanCode += code[i];
				}
			}
			else {
				cleanCode += code[i];
			}
		}
	}
	return cleanCode;
}

//----------------------------------------------------------------

void Tokenizer::skipWhitespace()
{
	char c;
	while (isspace(ss.peek())) {
		ss.get(c);
	}
}

//----------------------------------------------------------------

bool Tokenizer::isSymbol(char c)
{
	const std::string symbols = "()[]{},;=.*+-/&|~<>";
	return symbols.find(c) != std::string::npos;
}

//----------------------------------------------------------------
// constructor
//----------------------------------------------------------------

Tokenizer::Tokenizer(const std::string& filename)
{
	std::ifstream file(filename);

	if (!file.is_open()) {
		throw std::runtime_error("Tokenizer() : can't open file : " + filename);
	}

	if (!file.good()) {
		throw std::runtime_error("Tokenizer() : invalid file : " + filename);
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();

	std::string codeWithoutComments = removeComments(buffer.str());
	this->ss.str(codeWithoutComments);
}

//----------------------------------------------------------------
// public method
//----------------------------------------------------------------

bool Tokenizer::hasMoreTokens()
{
	skipWhitespace();
	return ss.peek() != EOF;
}

//----------------------------------------------------------------

void Tokenizer::advance()
{
	char nextChar = ss.peek();

	if (isdigit(nextChar)) {
		int intVal;
		this->ss >> intVal;

		this->token.type = Token::TokenType::INT_CONST;
		this->token.value = intVal;
	}
	else if (isSymbol(nextChar)) {
		this->token.type = Token::TokenType::SYMBOL;
		this->token.value = static_cast<char>(this->ss.get());
	}
	else if (nextChar == '"') {
		this->ss.get();

		std::string val = "";
		while (ss.peek() != '"') {
			val += ss.get();
		}

		ss.get();

		this->token.type = Token::TokenType::STRING_CONST;
		this->token.value = val;
	}
	else if(isalpha(nextChar)||nextChar=='_'){
		std::string val;
		
		while (isalnum(ss.peek()) || ss.peek() == '_') {
			// 유효한 문자이므로, val에 추가하고 스트림에서 제거(get)한다.
			val += ss.get();
		}

		if (Token::keywordMap.count(val)) {
			this->token.type = Token::TokenType::KEYWORD;
			this->token.value = Token::keywordMap.at(val);
		}
		else {
			this->token.type = Token::TokenType::IDENTIFIER;
			this->token.value = val;
		}
	}
}

//----------------------------------------------------------------
