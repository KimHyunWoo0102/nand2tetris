#include "CodeWriter.h"
#include<map>
#include<sstream>

//-------------------------------------------------------------------
// Maps VM memory segments to their corresponding assembly symbols.
//------------------------------------------------------------------- 

static const std::map<std::string, std::string>segmentMap = {
	{"local","LCL"},
	{"argument","ARG"},
	{"this","THIS"},
	{"that","THAT"},
};


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

//-------------------------------------------------------------------

void CodeWriter::writePushPop(VMParser::CMD_TYPE command, std::string segment, int index)
{
	std::string instruction = "";

	switch(command){
	case VMParser::CMD_TYPE::C_PUSH:
		instruction = makePushASMCode(segment, index);
		ofs << instruction;
		return;

	case VMParser::CMD_TYPE::C_POP:
		return;

	default:
		throw std::runtime_error("writePushPop() : invalid command ");
	}
}

//-------------------------------------------------------------------
//helper method
//-------------------------------------------------------------------

std::string CodeWriter::makePushASMCode(const std::string& segment, int index) {
	// RAM[sp] = RAM[segment_base + index]
	// sp++

	std::stringstream ss;

	// load RAM[seg + index]
	if (segment == "static") {
		// TODO: 나중에 구현
	}
	else if (segment == "constant") {
		ss << "@" << index << '\n';
		ss << "D=A\n";
	}
	else {
		auto segmentSymbol = segmentMap.at(segment);

		//addr = segment_base + index
		ss << "@" << segmentSymbol << '\n';
		ss << "D=M\n";
		ss << "@" << index << "\n";
		ss << "A=D+A\n";

		// D = RAM[addr]
		ss << "D=M\n";
	}

	// store to RAM[sp]

	ss << "@SP\n";
	ss << "A=M\n";
	ss << "M=D\n";

	// sp++
	ss << "@SP\n";
	ss << "M=M+1\n";

	return ss.str();
}

//-------------------------------------------------------------------

std::string CodeWriter::makePopASMCode(const std::string& segment, int index)
{
	std::stringstream ss;
	ss << "// pop " << segment << " " << index << "\n";

	if (segment == "constant") {
		// Case 1: constant (논리적 오류)
		throw std::runtime_error("Error: 'pop constant' is an invalid command.");
	}
	else if (segment == "static") {
		// Case 2: static (R13 불필요, 직접 처리)
		// TODO : 추후 구현
	}
	else { 
		// Case 3: local, argument, this, that, temp, pointer 등
		// segment_base + index addr -> R13
		std::string segSymbol = segmentMap.at(segment); // LCL, ARG 등
		ss << "@" << segSymbol << "\n";
		ss << "D=M\n";
		ss << "@" << index << "\n";
		ss << "D=D+A\n"; // D = target addr
		ss << "@R13\n";
		ss << "M=D\n";   

		// 2. RAM[SP] -> D register
		ss << "@SP\n";
		ss << "M=M-1\n";
		ss << "A=M\n";
		ss << "D=M\n";

		// 3. RAM[target addr] = D
		ss << "@R13\n";
		ss << "A=M\n";
		ss << "M=D\n";
	}

	return ss.str();
}
