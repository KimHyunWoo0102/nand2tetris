#include "VMTranslator.h"

VMTranslator::VMTranslator(const std::string& vm_filename)
	: m_filename(vm_filename),
	  m_parser(vm_filename),
	  m_codeWriter(vm_filename)
{
}

void VMTranslator::translate()
{
    // VM 파일의 모든 명령 처리
    while (m_parser.hasMoreLines()) {
        m_parser.advance();
        auto cmdType = m_parser.commandType();

        switch (cmdType) {
        case VMParser::CMD_TYPE::C_ARITHMETIC:
            m_codeWriter.writeArithmetic(m_parser.arg1());
            break;

        case VMParser::CMD_TYPE::C_PUSH:
        case VMParser::CMD_TYPE::C_POP:
            m_codeWriter.writePushPop(cmdType, m_parser.arg1(), m_parser.arg2());
            break;

            // TODO: 여기에 다른 명령어 타입(C_LABEL, C_GOTO 등)에 대한 case를 추가
        }
    }
}
