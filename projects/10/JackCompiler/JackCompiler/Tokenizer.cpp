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
