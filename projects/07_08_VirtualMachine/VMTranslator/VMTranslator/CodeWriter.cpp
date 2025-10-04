#include "CodeWriter.h"
#include <sstream>

//-------------------------------------------------------------------
// Maps VM memory segments to their corresponding assembly symbols.
//-------------------------------------------------------------------

static const std::map<std::string, std::string> segmentMap = {
    {"local",    "LCL"},
    {"argument", "ARG"},
    {"this",    "THIS"},
    {"that",    "THAT"}
};

//-------------------------------------------------------------------
// constructor && destructor
//-------------------------------------------------------------------

CodeWriter::CodeWriter(const std::string& inputVmFilename)
{
    // --- 1. 출력 파일(.asm) 경로 설정 ---
    auto dot_pos = inputVmFilename.find_last_of('.');
    if (dot_pos == std::string::npos)
        throw std::runtime_error("Invalid vm filename: " + inputVmFilename);

    std::string out_file_name = inputVmFilename.substr(0, dot_pos) + ".asm";
    ofs.open(out_file_name);
    if (!ofs.is_open()) {
        throw std::runtime_error("Failed to open output file: " + out_file_name);
    }

    // --- 2. static 변수용 파일 이름 설정 ---
    auto sep_pos = inputVmFilename.find_last_of("/\\");
    if (sep_pos == std::string::npos) {
        currentFileName = inputVmFilename.substr(0, dot_pos);
    }
    else {
        currentFileName = inputVmFilename.substr(sep_pos + 1, dot_pos - sep_pos - 1);
    }

    // Arithmetic command mapping
    arithmeticMap["add"] = [this]() { return this->makeAddASMCode(); };
    arithmeticMap["sub"] = [this]() { return this->makeSubASMCode(); };
    arithmeticMap["and"] = [this]() { return this->makeAndASMCode(); };
    arithmeticMap["or"] = [this]() { return this->makeOrASMCode(); };

    arithmeticMap["neg"] = [this]() { return this->makeNegASMCode(); };
    arithmeticMap["not"] = [this]() { return this->makeNotASMCode(); };

    arithmeticMap["eq"] = [this]() { return this->makeEqASMCode(); };
    arithmeticMap["gt"] = [this]() { return this->makeGTASMCode(); };
    arithmeticMap["lt"] = [this]() { return this->makeLTASMCode(); };
}

//-------------------------------------------------------------------

CodeWriter::~CodeWriter()
{
    ofs << "\n// Infinite loop to end the program\n"
        << "(END)\n"
        << "@END\n"
        << "0;JMP\n";

    ofs.close();
}

//-------------------------------------------------------------------

void CodeWriter::writeInit()
{
    // bootstrap code
    ofs << "// bootstrap code\n"
        << "@256\n"
        << "D=A\n"
        << "@SP\n"
        << "M=D\n\n";
}

//-------------------------------------------------------------------
// writeArithmetic
//-------------------------------------------------------------------

void CodeWriter::writeArithmetic(const std::string& command)
{
    if (arithmeticMap.count(command)) {
        ofs << "// " << command << "\n";
        ofs << arithmeticMap.at(command)();
    }
    else {
        throw std::runtime_error("writeArithmetic() : Invalid command " + command);
    }
}

//-------------------------------------------------------------------
// helper methods for arithmetic
//-------------------------------------------------------------------

std::string CodeWriter::makeAddASMCode()
{
    std::stringstream ss;
    ss << popStackToD() << "\n"
        << "@SP\n"
        << "A=M-1\n"
        << "M=M+D\n\n";
    return ss.str();
}

//-------------------------------------------------------------------

std::string CodeWriter::makeSubASMCode()
{
    std::stringstream ss;
    ss << popStackToD() << "\n"
        << "@SP\n"
        << "A=M-1\n"
        << "M=M-D\n\n";
    return ss.str();
}

//-------------------------------------------------------------------

std::string CodeWriter::makeAndASMCode()
{
    std::stringstream ss;
    ss << popStackToD() << "\n"
        << "@SP\n"
        << "A=M-1\n"
        << "M=M&D\n\n";
    return ss.str();
}

//-------------------------------------------------------------------

std::string CodeWriter::makeOrASMCode()
{
    std::stringstream ss;
    ss << popStackToD() << "\n"
        << "@SP\n"
        << "A=M-1\n"
        << "M=M|D\n\n";
    return ss.str();
}

//-------------------------------------------------------------------

std::string CodeWriter::makeNegASMCode()
{
    std::stringstream ss;
    ss << "@SP\n"
        << "A=M-1\n"
        << "M=-M\n\n";
    return ss.str();
}

//-------------------------------------------------------------------

std::string CodeWriter::makeNotASMCode()
{
    std::stringstream ss;
    ss << "@SP\n"
        << "A=M-1\n"
        << "M=!M\n\n";
    return ss.str();
}

//-------------------------------------------------------------------

std::string CodeWriter::makeEqASMCode()
{
    std::string unique_id = std::to_string(label_counter++);
    std::string trueLabel = "EQ_TRUE_" + unique_id;
    std::string endLabel = "EQ_END_" + unique_id;

    std::stringstream ss;
    ss << popStackToD() << "\n"
        << "@SP\n"
        << "A=M-1\n"
        << "D=M-D\n\n"

        << "@" << trueLabel << "\n"
        << "D;JEQ\n\n"

        << "@SP\n"
        << "A=M-1\n"
        << "M=0\n"
        << "@" << endLabel << "\n"
        << "0;JMP\n\n"

        << "(" << trueLabel << ")\n"
        << "@SP\n"
        << "A=M-1\n"
        << "M=-1\n\n"

        << "(" << endLabel << ")\n\n";
    return ss.str();
}

//-------------------------------------------------------------------

std::string CodeWriter::makeGTASMCode()
{
    std::string unique_id = std::to_string(label_counter++);
    std::string trueLabel = "GT_TRUE_" + unique_id;
    std::string endLabel = "GT_END_" + unique_id;

    std::stringstream ss;
    ss << popStackToD() << "\n"
        << "@SP\n"
        << "A=M-1\n"
        << "D=M-D\n\n"

        << "@" << trueLabel << "\n"
        << "D;JGT\n\n"

        << "@SP\n"
        << "A=M-1\n"
        << "M=0\n"
        << "@" << endLabel << "\n"
        << "0;JMP\n\n"

        << "(" << trueLabel << ")\n"
        << "@SP\n"
        << "A=M-1\n"
        << "M=-1\n\n"

        << "(" << endLabel << ")\n\n";
    return ss.str();
}

//-------------------------------------------------------------------

std::string CodeWriter::makeLTASMCode()
{
    std::string unique_id = std::to_string(label_counter++);
    std::string trueLabel = "LT_TRUE_" + unique_id;
    std::string endLabel = "LT_END_" + unique_id;

    std::stringstream ss;
    ss << popStackToD() << "\n"
        << "@SP\n"
        << "A=M-1\n"
        << "D=M-D\n\n"

        << "@" << trueLabel << "\n"
        << "D;JLT\n\n"

        << "@SP\n"
        << "A=M-1\n"
        << "M=0\n"
        << "@" << endLabel << "\n"
        << "0;JMP\n\n"

        << "(" << trueLabel << ")\n"
        << "@SP\n"
        << "A=M-1\n"
        << "M=-1\n\n"

        << "(" << endLabel << ")\n\n";
    return ss.str();
}

//-------------------------------------------------------------------
// writePushPop
//-------------------------------------------------------------------

void CodeWriter::writePushPop(VMParser::CMD_TYPE command, const std::string& segment, int index)
{
    std::string instruction;
    switch (command) {
    case VMParser::CMD_TYPE::C_PUSH:
        instruction = makePushASMCode(segment, index);
        ofs << instruction;
        return;
    case VMParser::CMD_TYPE::C_POP:
        instruction = makePopASMCode(segment, index);
        ofs << instruction;
        return;
    default:
        throw std::runtime_error("writePushPop() : invalid command ");
    }
}

//-------------------------------------------------------------------
// helper methods for push/pop
//-------------------------------------------------------------------

std::string CodeWriter::makePushASMCode(const std::string& segment, int index)
{
    std::stringstream ss;
    ss << "// push " << segment << " " << index << "\n";

    if (segment == "static") {
        ss  << "@" << currentFileName << "." << index << '\n'
            << "D=M\n\n";
    }
    else if (segment == "constant") {
        ss  << "@" << index << "\n"
            << "D=A\n\n";
    }
    else if (segment == "pointer") {
        ss  << (index == 0 ? "@THIS\n" : "@THAT\n")
            << "D=M\n\n";
    }
    else if (segment == "temp") {
        ss  << "@R5\n"
            << "D=A\n"
            << "@" << index << "\n"
            << "A=D+A\n"
            << "D=M\n\n";
    }
    else {
        auto segmentSymbol = segmentMap.at(segment);
        ss  << "@" << segmentSymbol << "\n"
            << "D=M\n"
            << "@" << index << "\n"
            << "A=D+A\n"
            << "D=M\n\n";
    }

    ss << pushDToStack() << "\n";
    return ss.str();
}

//-------------------------------------------------------------------

std::string CodeWriter::makePopASMCode(const std::string& segment, int index)
{
    std::stringstream ss;
    ss << "// pop " << segment << " " << index << "\n";

    if (segment == "constant") {
        throw std::runtime_error("Error: 'pop constant' is an invalid command.");
    }
    else if (segment == "static") {
        ss  << popStackToD()
            // FileName.index 변수에 D 값을 저장
            << "@" << currentFileName << "." << index << "\n"
            << "M=D\n";
    }
    else if (segment == "pointer") {
        ss  << popStackToD()
            // index가 0이면 THIS, 1이면 THAT에 D 값을 저장
            << (index == 0 ? "@THIS\n" : "@THAT\n")
            << "M=D\n";
    }
    else if (segment == "temp") {
        ss  << "@R5\n"
            << "D=A\n"
            << "@" << index << "\n"
            << "D=D+A\n"
            << "@R13\n"
            << "M=D\n"

            << popStackToD()

            << "@R13\n"
            << "A=M\n"
            << "M=D\n";
    }

    else {// local, argument ,this, that
        std::string segSymbol = segmentMap.at(segment);
        ss  << "@" << segSymbol << "\n"
            << "D=M\n"
            << "@" << index << "\n"
            << "D=D+A\n"
            << "@R13\n"
            << "M=D\n\n"

            << popStackToD() << "\n"

            << "@R13\n"
            << "A=M\n"
            << "M=D\n\n";
    }

    return ss.str();
}

//-------------------------------------------------------------------
// low-level stack helpers
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
