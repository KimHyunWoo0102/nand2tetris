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

        std::cout << "Translation completed. Check the .asm file." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}