#include "VMTranslator.h"

VMTranslator::VMTranslator(const std::string& vm_filename)
	: m_filename(vm_filename),
	  m_parser(vm_filename),
	  m_codeWriter(vm_filename)
{
}

void VMTranslator::translate()
{
    // VM ������ ��� ��� ó��
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

            // TODO: ���⿡ �ٸ� ��ɾ� Ÿ��(C_LABEL, C_GOTO ��)�� ���� case�� �߰�
        }
    }
}
