#include"Parser.h"
#include"CodeWriter.h"

int main() {
    try {
        std::string filename;
        std::cout << "Enter VM filename (.vm): ";
        std::cin >> filename;

        // VM �ļ� �ʱ�ȭ
        VMParser::Parser parser(filename);

        // CodeWriter �ʱ�ȭ (��� asm ���� ������)
        CodeWriter cw(filename);

        // VM ������ ��� ��� ó��
        while (parser.hasMoreLines()) {
            parser.advance();
            auto cmdType = parser.commandType();

            if (cmdType == VMParser::CMD_TYPE::C_ARITHMETIC) {
                std::string arg1 = parser.arg1();
                cw.writeArithmetic(arg1);
            }
            else if (cmdType == VMParser::CMD_TYPE::C_PUSH ||
                cmdType == VMParser::CMD_TYPE::C_POP) {
                std::string segment = parser.arg1();
                int index = parser.arg2();
                cw.writePushPop(cmdType, segment, index);
            }
            else {
                // TODO: label, goto, function, return ���� �ٸ� ��ɾ� ������ ó��
            }
        }

        std::cout << "Translation completed. Check the .asm file." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}