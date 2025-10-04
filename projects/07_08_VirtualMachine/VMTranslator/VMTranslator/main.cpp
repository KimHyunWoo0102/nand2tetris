#include"VMTranslator.h"
#include<iostream>
#include<string>

using namespace std;

int main() {
    try {
        std::string filename;
        std::cout << "Enter VM filename (.vm): ";
        std::cin >> filename;

        VMTranslator translator(filename); // ¼öÁ¤
        translator.translate();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}