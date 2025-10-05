#include"VMTranslator.h"
#include<iostream>
#include<string>

using namespace std;

int main() {
    try {
        std::cout << "Program started... \n";

        std::string filename;
        std::cout << "Enter source (.vm) or (directory) : ";
        std::cin >> filename;
        std::cout << "Input received: " << filename << "\n";
     
        VMTranslator translator(filename);
        std::cout << "Translator constructed\n";

        std::cout << "start translating...\n";  

        translator.translate();
        std::cout << "Translation done\n";

        std::cin.get(); std::cin.get();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cin.get(); std::cin.get();
        return 1;
    }

    return 0;
}