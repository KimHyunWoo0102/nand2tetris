#include "CodeWriter.h"

CodeWriter::CodeWriter(std::string& filename)
{
	auto dot_pos = filename.find_last_of('.');

	if (dot_pos == std::string::npos)
		throw std::runtime_error("CodeWriter() : invalid filename " + filename);

	auto out_file_name = filename.substr(0, dot_pos) + ".asm";

	std::cout << out_file_name;


}

CodeWriter::~CodeWriter()
{
}

void CodeWriter::writeArithmetic(std::string& command)
{
}
