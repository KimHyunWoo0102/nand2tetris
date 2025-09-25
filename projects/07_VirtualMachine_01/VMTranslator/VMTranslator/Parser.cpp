#include "Parser.h"

VMParser::Parser::Parser(std::string& filename)
	:ist(filename)
{
	if (!ist.is_open()) {
		throw std::runtime_error("Error : can't find file " + filename);
	}
}

bool VMParser::Parser::hasMoreLines()
{
	return ist.peek() != EOF;
}

void VMParser::Parser::advance()
{
	while (std::getline(ist, current_command)) {
		auto annotation_pos = current_command.find("//");

		if (annotation_pos != std::string::npos) {
			current_command = current_command.substr(0, annotation_pos);
		}
		
		auto first = current_command.find_first_not_of(" \t\n\r\f\v");

		if (first == std::string::npos) {
			continue;
		}

		auto last = current_command.find_last_not_of(" \t\n\r\f\v");

		if (last != std::string::npos) {
			current_command = current_command.substr(first, last - first + 1);
		}

		if (current_command.size())return;
	}
}
