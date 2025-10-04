#include "VMTranslator.h"
#include<filesystem>

namespace fs = std::filesystem;

// VMTranslator.cpp

#include "VMTranslator.h"
#include <filesystem>
#include "Parser.h" // Parser�� �ʿ��մϴ�.

namespace fs = std::filesystem;

VMTranslator::VMTranslator(const std::string& inputPath)
{
    fs::path path(inputPath);
    std::string output_path_str;

    if (!fs::exists(path)) {
        throw std::runtime_error("Input path does not exist: " + inputPath);
    }

    // 1. �Է� ��θ� �м��Ͽ� '��� ���'�� '������ ���� ���'�� �����մϴ�.
    if (fs::is_directory(path)) {
        // �Է��� ���丮�� ���
        output_path_str = path.string() + "/" + path.filename().string() + ".asm"; // .stem() ��� .filename() ���

        for (const auto& entry : fs::directory_iterator(path)) {
            if (entry.path().extension() == ".vm") {
                m_vmFiles.push_back(entry.path().string());
            }
        }
    }
    else if (path.extension() == ".vm") {
        // �Է��� ���� .vm ������ ���
        output_path_str = path.parent_path().string() + "/" + path.stem().string() + ".asm";
        m_vmFiles.push_back(path.string());
    }
    else {
        throw std::runtime_error("Invalid input path: Not a .vm file or directory.");
    }

    if (m_vmFiles.empty()) {
        throw std::runtime_error("No .vm files found.");
    }

    // 2. ������ '��� ���'�� CodeWriter�� �ʱ�ȭ�մϴ�.
    // (m_codeWriter�� ����� �⺻ �����ڷ� ����Ǿ� �־�� �մϴ�)
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
                // ... ���⿡ label, goto, if-goto, function, call, return�� ���� case �߰� ...
            }
        }
    }

