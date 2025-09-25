#include "CodeWriter.h"

//-------------------------------------------------------------------
//constructor && destructor
//-------------------------------------------------------------------

CodeWriter::CodeWriter(std::string& filename)
{
	auto dot_pos = filename.find_last_of('.');

	if (dot_pos == std::string::npos)
		throw std::runtime_error("CodeWriter() : invalid filename " + filename);

	auto out_file_name = filename.substr(0, dot_pos) + ".asm";

	ofs.open(out_file_name);

	if (!ofs.is_open()) {
		throw std::runtime_error("CodeWriter(): Failed to open output file " + out_file_name);
	}
}

//-------------------------------------------------------------------

CodeWriter::~CodeWriter()
{
	ofs.close();
}

//-------------------------------------------------------------------

void CodeWriter::writeArithmetic(std::string& command)
{
}
