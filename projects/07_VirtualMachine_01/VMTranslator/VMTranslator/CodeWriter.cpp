#include "CodeWriter.h"
#include<sstream>

//-------------------------------------------------------------------
// Maps VM memory sents to their corresponding assembly symbols.
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

	//----------------------------------------------------------------
	ofs << "@256\nD=A\n@SP\nM=D\n";
	// Arithmetic Command Dispatch Table Initialization
	//================================================================

	// 이항 연산자 매핑
	arithmeticMap["add"] = [this]() { return this->makeAddASMCode(); };
	arithmeticMap["sub"] = [this]() { return this->makeSubASMCode(); };
	arithmeticMap["and"] = [this]() { return this->makeAndASMCode(); };
	arithmeticMap["or"] = [this]() { return this->makeOrASMCode(); };

	// 단항 연산자 매핑
	arithmeticMap["neg"] = [this]() { return this->makeNegASMCode(); };
	arithmeticMap["not"] = [this]() { return this->makeNotASMCode(); };

	// 비교 연산자 매핑
	arithmeticMap["eq"] = [this]() { return this->makeEqASMCode(); };
	arithmeticMap["gt"] = [this]() { return this->makeGTASMCode(); };
	arithmeticMap["lt"] = [this]() { return this->makeLTASMCode(); };

}

//-------------------------------------------------------------------

CodeWriter::~CodeWriter()
{
	ofs.close();
}

//-------------------------------------------------------------------
// writeArithmetic
//-------------------------------------------------------------------

void CodeWriter::writeArithmetic(std::string& command)
{
	if (arithmeticMap.count(command)) {
		ofs << "//" << command << '\n';
		ofs << arithmeticMap.at(command)();
	}
	else {
		throw std::runtime_error("writeArithmetic() : Invalid command " + command);
	}
	
}

//-------------------------------------------------------------------
// helper method
//-------------------------------------------------------------------

std::string CodeWriter::makeAddASMCode()
{
	std::stringstream ss;
	ss << popStackToD();
	ss << "@SP\n";
	ss << "A=M-1\n";
	ss << "M=M+D\n";
	return ss.str();
}

//-------------------------------------------------------------------

std::string CodeWriter::makeSubASMCode()
{
	std::stringstream ss;
	ss << popStackToD();
	ss << "@SP\n";
	ss << "A=M-1\n";
	ss << "M=M-D\n";
	return ss.str();
}

//-------------------------------------------------------------------

std::string CodeWriter::makeAndASMCode()
{
	std::stringstream ss;
	ss << popStackToD();
	ss << "@SP\n";
	ss << "A=M-1\n";
	ss << "M=M&D\n";
	return ss.str();
}

//-------------------------------------------------------------------

std::string CodeWriter::makeOrASMCode()
{
	std::stringstream ss;
	ss << popStackToD();
	ss << "@SP\n";
	ss << "A=M-1\n";
	ss << "M=M|D\n";
	return ss.str();
}

//-------------------------------------------------------------------

std::string CodeWriter::makeNegASMCode()
{
	std::stringstream ss;
	ss << "@SP\n";
	ss << "A=M-1\n";
	ss << "M=-M\n";
	return ss.str();
}

//-------------------------------------------------------------------

std::string CodeWriter::makeNotASMCode()
{
	std::stringstream ss;
	ss << "@SP\n";
	ss << "A=M-1\n";
	ss << "M=!M\n";
	return ss.str();
}

//-------------------------------------------------------------------

std::string CodeWriter::makeEqASMCode()
{
	// 1. 이번 'eq' 연산에서 사용할 고유 ID를 하나만 생성합니다.
	std::string unique_id = std::to_string(label_counter);
	std::string trueLabel = "EQ_TRUE_" + unique_id;
	std::string endLabel = "EQ_END_" + unique_id;
	label_counter++; 

	std::stringstream ss;
	ss << "// eq\n";

	// D=x-y
	ss << popStackToD();      // D = y
	ss << "@SP\n";
	ss << "A=M-1\n";          // A -> x 주소
	ss << "D=M-D\n";          // D = x - y

	ss << "@" << trueLabel << "\n";
	ss << "D;JEQ\n";          // if (x == y) goto EQ_TRUE_...

	ss << "@SP\n";
	ss << "A=M-1\n";
	ss << "M=0\n";            
	ss << "@" << endLabel << "\n";
	ss << "0;JMP\n";        

	ss << "(" << trueLabel << ")\n";
	ss << "@SP\n";
	ss << "A=M-1\n";
	ss << "M=-1\n";           

	ss << "(" << endLabel << ")\n";

	return ss.str();
}

//-------------------------------------------------------------------

std::string CodeWriter::makeGTASMCode()
{
	std::string unique_id = std::to_string(label_counter);
	std::string trueLabel = "GT_TRUE_" + unique_id;
	std::string endLabel = "GT_END_" + unique_id;
	label_counter++;

	std::stringstream ss;
	ss << "// GT\n";

	// D=x-y
	ss << popStackToD();      // D = y
	ss << "@SP\n";
	ss << "A=M-1\n";          // A -> x 주소
	ss << "D=M-D\n";          // D = x - y

	ss << "@" << trueLabel << "\n";
	ss << "D;JGT\n";          // if (x < y) goto EQ_TRUE_...

	ss << "@SP\n";
	ss << "A=M-1\n";
	ss << "M=0\n";
	ss << "@" << endLabel << "\n";
	ss << "0;JMP\n";

	ss << "(" << trueLabel << ")\n";
	ss << "@SP\n";
	ss << "A=M-1\n";
	ss << "M=-1\n";

	ss << "(" << endLabel << ")\n";

	return ss.str();
}

//-------------------------------------------------------------------

std::string CodeWriter::makeLTASMCode()
{
	std::string unique_id = std::to_string(label_counter);
	std::string trueLabel = "LT_TRUE_" + unique_id;
	std::string endLabel = "LT_END_" + unique_id;
	label_counter++;

	std::stringstream ss;
	ss << "// LT\n";

	// D=x-y
	ss << popStackToD();      // D = y
	ss << "@SP\n";
	ss << "A=M-1\n";          // A -> x 주소
	ss << "D=M-D\n";          // D = x - y

	ss << "@" << trueLabel << "\n";
	ss << "D;JLT\n";          // if (x < y) goto EQ_TRUE_...

	ss << "@SP\n";
	ss << "A=M-1\n";
	ss << "M=0\n";
	ss << "@" << endLabel << "\n";
	ss << "0;JMP\n";

	ss << "(" << trueLabel << ")\n";
	ss << "@SP\n";
	ss << "A=M-1\n";
	ss << "M=-1\n";

	ss << "(" << endLabel << ")\n";

	return ss.str();
}

//-------------------------------------------------------------------
// writePushPop
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
		instruction = makePopASMCode(segment, index);
		ofs << instruction;
		return; // return 추가

	default:
		throw std::runtime_error("writePushPop() : invalid command ");
	}
}

//-------------------------------------------------------------------
// helper method
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

	ss << pushDToStack();

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
		ss << popStackToD();

		// 3. RAM[target addr] = D
		ss << "@R13\n";
		ss << "A=M\n";
		ss << "M=D\n";
	}

	return ss.str();
}

//-------------------------------------------------------------------

std::string CodeWriter::popStackToD()
{
	return "@SP\nM=M-1\nA=M\nD=M\n";
}

//-------------------------------------------------------------------

std::string CodeWriter::pushDToStack()
{
	return "@SP\nA=M\nM=D\n@SP\nM=M+1\n";
}

