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

void Tokenizer::readNextToken()
{
	char nextChar = ss.peek();

	if (isdigit(nextChar)) {
		int intVal;
		this->ss >> intVal;

		this->nextToken.type = Token::TokenType::INT_CONST;
		this->nextToken.value = intVal;
	}
	else if (isSymbol(nextChar)) {
		this->nextToken.type = Token::TokenType::SYMBOL;
		this->nextToken.value = static_cast<char>(this->ss.get());
	}
	else if (nextChar == '"') {
		this->ss.get();

		std::string val = "";
		while (ss.peek() != '"') {
			val += ss.get();
		}

		ss.get();

		this->nextToken.type = Token::TokenType::STRING_CONST;
		this->nextToken.value = val;
	}
	else if (isalpha(nextChar) || nextChar == '_') {
		std::string val;

		while (isalnum(ss.peek()) || ss.peek() == '_') {
			// 유효한 문자이므로, val에 추가하고 스트림에서 제거(get)한다.
			val += ss.get();
		}

		if (Token::keywordMap.count(val)) {
			this->nextToken.type = Token::TokenType::KEYWORD;
			this->nextToken.value = Token::keywordMap.at(val);
		}
		else {
			this->nextToken.type = Token::TokenType::IDENTIFIER;
			this->nextToken.value = val;
		}
	}
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
	readNextToken();
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
	this->currentToken = this->nextToken;
	this->readNextToken();
}

//----------------------------------------------------------------
// getter
//----------------------------------------------------------------

int Tokenizer::intVal() const {
	if (this->currentToken.type != Token::TokenType::INT_CONST) {
		throw std::logic_error("intVal() : Token is not an INT_CONST");
	}

	return std::get<int>(this->currentToken.value);
}

//----------------------------------------------------------------

char Tokenizer::symbol() const
{
	if (this->currentToken.type != Token::TokenType::SYMBOL) {
		throw std::logic_error("symbol() : Token is not a SYMBOL");
	}
	return std::get<char>(this->currentToken.value);
}

//----------------------------------------------------------------

const std::string& Tokenizer::identifier() const
{
	if (this->currentToken.type != Token::TokenType::IDENTIFIER) {
		throw std::logic_error("identifier() : Token is not an IDENTIFIER");
	}
	return std::get<std::string>(this->currentToken.value);
}

//----------------------------------------------------------------

const std::string& Tokenizer::stringVal() const
{
	if (this->currentToken.type != Token::TokenType::STRING_CONST) {
		throw std::logic_error("stringVal() : Token is not a STRING_CONST");
	}
	return std::get<std::string>(this->currentToken.value);
}

//----------------------------------------------------------------

Token::KeywordType Tokenizer::keyword() const
{
	if (this->currentToken.type != Token::TokenType::KEYWORD) {
		throw std::logic_error("keyword() : Token is not a KEYWORD");
	}
	return std::get<Token::KeywordType>(this->currentToken.value);
}

//----------------------------------------------------------------

Token::TokenType Tokenizer::tokenType() const
{
	return this->currentToken.type;
}

//----------------------------------------------------------------
