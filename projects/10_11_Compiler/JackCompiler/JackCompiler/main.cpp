#include <iostream>
#include <string>
#include <stdexcept> // For std::exception

#include "JackCompiler.h" // JackCompiler Ŭ���� ���

int main(int argc, char* argv[]) {
    // 1. Ŀ�ǵ� ���� ���ڰ� ��Ȯ�� �ϳ����� Ȯ�� (���α׷� �̸� ����)
    if (argc != 2) {
        // cerr�� ���� �޽����� ����� �� �ַ� ���
        std::cerr << "Usage: " << argv[0] << " <filename.jack or directory>" << std::endl;
        return 1; // ���� ���� �ڵ� ��ȯ
    }

    try {
        // 2. JackCompiler ��ü ���� (�����ڿ��� ��� ó��)
        JackCompiler compiler(argv[1]);

        // 3. ������ ����
        compiler.compile();

        std::cout << "Compilation successful." << std::endl;
        
    }
    catch (const std::exception& e) {
        // ������ �� �߻��� ���� ó�� (���� ���� ����, ���� ���� ��)
        std::cerr << "Compilation Error: " << e.what() << std::endl;
        return 1; // ���� ���� �ڵ� ��ȯ
    }

    return 0; // ���� ����
}