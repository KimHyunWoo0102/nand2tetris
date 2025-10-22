#pragma once

#include <fstream>
#include <string>
#include <stdexcept>

enum class Segment { CONSTANT, ARGUMENT, LOCAL, STATIC, THIS, THAT, POINTER, TEMP };
enum class Command { ADD, SUB, NEG, EQ, GT, LT, AND, OR, NOT };

class VMWriter {
private:
    std::ofstream file;
    std::string segmentToString(Segment seg);
    std::string commandToString(Command command);

public:
    VMWriter(const std::string& filename);

    void writePush(Segment segment, size_t index);
    void writePop(Segment segment, size_t index);

    void writeArithmetic(Command command);

    void writeLabel(const std::string& label);
    void writeGoto(const std::string& label);
    void writeIf(const std::string& label);

    void writeCall(const std::string& functionName, size_t nArgs);
    void writeFunction(const std::string& functionName, size_t nLocals);
    void writeReturn();

    void close();
};