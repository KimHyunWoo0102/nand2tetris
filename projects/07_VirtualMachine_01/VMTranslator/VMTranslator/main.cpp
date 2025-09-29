#include"Parser.h"
#include"CodeWriter.h"

int main() {
    try {
        std::string filename;
        std::cout << "Enter VM filename (.vm): ";
        std::cin >> filename;

        // VM 파서 초기화
        VMParser::Parser parser(filename);

        // CodeWriter 초기화 (출력 asm 파일 생성됨)
        CodeWriter cw(filename);

        // VM 파일의 모든 명령 처리
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
                // TODO: label, goto, function, return 같은 다른 명령어 지원시 처리
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