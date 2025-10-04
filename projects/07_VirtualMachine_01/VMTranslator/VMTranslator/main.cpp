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

        std::cout << "Translation completed. Check the .asm file." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}