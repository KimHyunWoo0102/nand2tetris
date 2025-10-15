
#include <iostream>
#include<filesystem>
#include "Tokenizer.h" // Tokenizer와 keywordToString 함수가 포함된 헤더
#include"CompilationEngine.h"

int main() {
    try {
        std::string filename;
        std::cout << "Enter .jack file name: ";
        std::cin >> filename;

        Tokenizer tn(filename);
        tn.print();

        std::filesystem::path fp(filename);
        fp.replace_extension(".xml");
        auto outputPath = fp.string();

        CompilationEngine cm(tn,outputPath);

        cm.compile();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}