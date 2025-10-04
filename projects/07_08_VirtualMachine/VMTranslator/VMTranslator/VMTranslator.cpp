#include "VMTranslator.h"
#include<filesystem>

namespace fs = std::filesystem;

// VMTranslator.cpp

#include "VMTranslator.h"
#include <filesystem>
#include "Parser.h" // Parser도 필요합니다.

namespace fs = std::filesystem;

VMTranslator::VMTranslator(const std::string& inputPath)
{
    fs::path path(inputPath);
    std::string output_path_str;

    if (!fs::exists(path)) {
        throw std::runtime_error("Input path does not exist: " + inputPath);
    }

    // 1. 입력 경로를 분석하여 '출력 경로'와 '번역할 파일 목록'을 결정합니다.
    if (fs::is_directory(path)) {
        // 입력이 디렉토리인 경우
        output_path_str = path.string() + "/" + path.filename().string() + ".asm"; // .stem() 대신 .filename() 사용

        for (const auto& entry : fs::directory_iterator(path)) {
            if (entry.path().extension() == ".vm") {
                m_vmFiles.push_back(entry.path().string());
            }
        }
    }
    else if (path.extension() == ".vm") {
        // 입력이 단일 .vm 파일인 경우
        output_path_str = path.parent_path().string() + "/" + path.stem().string() + ".asm";
        m_vmFiles.push_back(path.string());
    }
    else {
        throw std::runtime_error("Invalid input path: Not a .vm file or directory.");
    }

    if (m_vmFiles.empty()) {
        throw std::runtime_error("No .vm files found.");
    }

    // 2. 결정된 '출력 경로'로 CodeWriter를 초기화합니다.
    // (m_codeWriter는 헤더에 기본 생성자로 선언되어 있어야 합니다)
    m_codeWriter.setOutputPath(output_path_str);
}

void VMTranslator::translate()
{
    m_codeWriter.writeInit();
    m_codeWriter.writeCall("Sys.init", 0);

    for (const auto& vmFile : m_vmFiles) {
        m_code.setCurrentFileName(vmFile);

        VMParser::Parser parser(vmFile);

        while (parser.hasMoreLines()) {
            parser.advance();

            auto cmdType = parser.commandType();

            switch (cmdType) {
            case VMParser::CMD_TYPE::C_ARITHMETIC:
                m_codeWriter.writeArithmetic(parser.arg1());
                break;
            case VMParser::CMD_TYPE::C_PUSH:
            case VMParser::CMD_TYPE::C_POP:
                m_codeWriter.writePushPop(cmdType, parser.arg1(), parser.arg2());
                break;
                // ... 여기에 label, goto, if-goto, function, call, return에 대한 case 추가 ...
            }
        }
    }

