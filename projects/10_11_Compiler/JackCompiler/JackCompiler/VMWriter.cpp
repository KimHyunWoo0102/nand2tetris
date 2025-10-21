#include "VMWriter.h"

// --- Constructor ---
VMWriter::VMWriter(const std::string& filename) : file(filename) {
    if (!file.is_open()) {
        throw std::runtime_error("VMWriter Error: Cannot open output file: " + filename);
    }
}

// --- Private Helper Functions ---

std::string VMWriter::segmentToString(Segment seg) {
    switch (seg) {
    case Segment::CONSTANT:  return "constant";
    case Segment::ARGUMENT:  return "argument";
    case Segment::LOCAL:     return "local";
    case Segment::STATIC:    return "static";
    case Segment::THIS:      return "this";
    case Segment::THAT:      return "that";
    case Segment::POINTER:   return "pointer";
    case Segment::TEMP:      return "temp";
    default:                 throw std::logic_error("Unknown segment in segmentToString");
    }
}

//--------------------------------

std::string VMWriter::commandToString(Command command) {
    switch (command) {
    case Command::ADD: return "add";
    case Command::SUB: return "sub";
    case Command::NEG: return "neg";
    case Command::EQ:  return "eq";
    case Command::GT:  return "gt";
    case Command::LT:  return "lt";
    case Command::AND: return "and";
    case Command::OR:  return "or";
    case Command::NOT: return "not";
    default:           throw std::logic_error("Unknown command in commandToString");
    }
}

// --- Public Write Methods ---

void VMWriter::writePush(Segment segment, size_t index) {
    file << "push " << segmentToString(segment) << " " << index << "\n";
}

//--------------------------------

void VMWriter::writePop(Segment segment, size_t index) {
    file << "pop " << segmentToString(segment) << " " << index << "\n";
}

//--------------------------------

void VMWriter::writeArithmetic(Command command) {
    file << commandToString(command) << "\n";
}

//--------------------------------

void VMWriter::writeLabel(const std::string& label) {
    file << "label " << label << "\n";
}

//--------------------------------

void VMWriter::writeGoto(const std::string& label) {
    file << "goto " << label << "\n";
}

//--------------------------------

void VMWriter::writeIf(const std::string& label) {
    file << "if-goto " << label << "\n";
}

//--------------------------------

void VMWriter::writeCall(const std::string& functionName, size_t nArgs) {
    file << "call " << functionName << " " << nArgs << "\n";
}

//--------------------------------

void VMWriter::writeFunction(const std::string& functionName, size_t nLocals) {
    file << "function " << functionName << " " << nLocals << "\n";
}

//--------------------------------

void VMWriter::writeReturn() {
    file << "return\n";
}

// --- File Management ---

void VMWriter::close() {
    if (file.is_open()) {
        file.close();
    }
}
