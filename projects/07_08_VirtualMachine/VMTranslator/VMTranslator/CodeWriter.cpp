#include "CodeWriter.h"
#include <sstream>
#include<filesystem>

namespace fs = std::filesystem;
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

CodeWriter::CodeWriter()
{
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
        << "(PROGRAM_END)\n"
        << "@PROGRAM_END\n"
        << "0;JMP\n";

    ofs.close();
}

//-------------------------------------------------------------------
// Setup && ASM bootstrap initialization
//-------------------------------------------------------------------

void CodeWriter::setOutputPath(const std::string& outputPath)
{
    this->outputPath = outputPath;

    ofs.open(this->outputPath);
    
    if (!ofs.is_open()) {
        throw std::runtime_error("Failed to open output file: " + outputPath);
    }
}

//-------------------------------------------------------------------

void CodeWriter::setCurrentFileName(const std::string& vmFilePath)
{
    // ✨ 전체 경로(vmFilePath)에서 확장자를 제외한 파일 이름만 추출합니다.
    this->currentFileName = fs::path(vmFilePath).stem().string();
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
// private Branch helper method
//-------------------------------------------------------------------

std::string CodeWriter::generateLabelName(const std::string& label) {
    if (currentFunctionName.empty()) {
        return label; // 함수가 없으면 레이블 이름 그대로 사용
    }
    return currentFunctionName + "$" + label; // 함수가 있으면 FunctionName$Label 형식 사용
}

//-------------------------------------------------------------------
// Write branch instructions 
//-------------------------------------------------------------------

void CodeWriter::writeLabel(const std::string& label)
{
    ofs << "// label " << label << '\n';
    
    ofs << "(" << generateLabelName(label) << ")\n";
}

//-------------------------------------------------------------------

void CodeWriter::writeGoto(const std::string& label)
{
    ofs << "// goto " << label << "\n";
    ofs << "@" << generateLabelName(label) << "\n";
    ofs << "0;JMP\n";
}

//-------------------------------------------------------------------

void CodeWriter::writeIf(const std::string& label)
{
    ofs << "// if-goto " << label << '\n';
    ofs << popStackToD();

    ofs << "@" << generateLabelName(label) << "\n";
    ofs << "D;JNE\n";
}

//-------------------------------------------------------------------

void CodeWriter::writeFunction(const std::string& functionName, int numLocals)
{
    this->currentFunctionName = functionName;
    this->label_counter = 0; // 함수가 바뀌었으니 고유 레이블 카운터도 리셋

    ofs << "// function " << functionName << " " << numLocals << "\n"
        << "(" << functionName << ")\n";

    for (int i = 0; i < numLocals; ++i) {
        ofs << "@SP\n"
            << "A=M\n"
            << "M=0\n" // 스택에 0을 push
            << "@SP\n"
            << "M=M+1\n";
    }
}
  
//-------------------------------------------------------------------

void CodeWriter::writeCall(const std::string& functionName, int numArgs)
{
    std::string returnLabel = functionName + "$ret." + std::to_string(label_counter++);

    ofs << "// call " << functionName << " " << numArgs << "\n";

    // push return address
    ofs << "@" << returnLabel << '\n'
        << "D=A\n" << pushDToStack();
   
    // push LCL,ARG,THIS,THAT
    ofs << "@LCL\nD=M\n" << pushDToStack();
    ofs << "@ARG\nD=M\n" << pushDToStack();
    ofs << "@THIS\nD=M\n" << pushDToStack();
    ofs << "@THAT\nD=M\n" << pushDToStack();

    // reset ARG
    ofs << "@SP\n";
    ofs << "D=M\n";
    ofs << "@" << 5 + numArgs << '\n';
    ofs << "D=D-A\n";
    ofs << "@ARG\nM=D\n";

    //reset LCL;
    ofs << "@SP\nD=M\n@LCL\nM=D\n";
    
    ofs << "@" << functionName << '\n'
        << "0;JMP\n";

    ofs << "(" << returnLabel << ")\n";
}

//-------------------------------------------------------------------

void CodeWriter::writeReturn()
{
    ofs << "// return\n";

    // FRAME = LCL (use R13 as a temporary variable)
    ofs << "@LCL\n"
        << "D=M\n"
        << "@R13\n"
        << "M=D\n";

    // RET = *(FRAME - 5) (use R14 for return address)
    ofs << "@5\n"
        << "A=D-A\n" // D still holds the value of LCL from above
        << "D=M\n"
        << "@R14\n"
        << "M=D\n";

    // *ARG = pop()
    ofs << popStackToD()
        << "@ARG\n"
        << "A=M\n"
        << "M=D\n";

    ofs << "@ARG\n"
        << "D=M+1\n"
        << "@SP\n"
        << "M=D\n";

    // Restore THAT, THIS, ARG, LCL
    ofs << "@R13\nAM=M-1\nD=M\n@THAT\nM=D\n"
        << "@R13\nAM=M-1\nD=M\n@THIS\nM=D\n"
        << "@R13\nAM=M-1\nD=M\n@ARG\nM=D\n"
        << "@R13\nAM=M-1\nD=M\n@LCL\nM=D\n";

    // goto RET
    ofs << "@R14\n"
        << "A=M\n"
        << "0;JMP\n";
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

//-------------------------------------------------------------------
