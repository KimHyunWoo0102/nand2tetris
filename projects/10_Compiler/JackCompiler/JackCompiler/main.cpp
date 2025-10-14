#include"Tokenizer.h"

std::string keywordToString(Token::KeywordType kw) {
    switch (kw) {
        // Ŭ����, �����ƾ ����
    case Token::KeywordType::CLASS:       return "class";
    case Token::KeywordType::METHOD:      return "method";
    case Token::KeywordType::FUNCTION:    return "function";
    case Token::KeywordType::CONSTRUCTOR: return "constructor";

        // ���� Ÿ��
    case Token::KeywordType::INT:         return "int";
    case Token::KeywordType::BOOLEAN:     return "boolean";
    case Token::KeywordType::CHAR:        return "char";
    case Token::KeywordType::VOID:        return "void";

        // ���� ����
    case Token::KeywordType::VAR:         return "var";
    case Token::KeywordType::STATIC:      return "static";
    case Token::KeywordType::FIELD:       return "field";

        // ����
    case Token::KeywordType::LET:         return "let";
    case Token::KeywordType::DO:          return "do";
    case Token::KeywordType::IF:          return "if";
    case Token::KeywordType::ELSE:        return "else";
    case Token::KeywordType::WHILE:       return "while";
    case Token::KeywordType::RETURN:      return "return";

        // ��� Ű����
    case Token::KeywordType::TRUE:        return "true";
    case Token::KeywordType::FALSE:       return "false";
    case Token::KeywordType ::NULL_KEYWORD:       return "null";
    case Token::KeywordType::THIS:        return "this";

        // ���� ó��
    default:                          return "Unknown Keyword";
    }
}

#include <iostream>
#include "Tokenizer.h" // Tokenizer�� keywordToString �Լ��� ���Ե� ���

int main() {
    std::string filename;
    std::cout << "Enter .jack file name: ";
    std::cin >> filename;

    try {
        Tokenizer tn(filename);

        while (tn.hasMoreTokens()) {
            tn.advance();
            auto token = tn.peekToken();

            switch (token.type) {
            case Token::TokenType::INT_CONST:
                std::cout << "INT_CONST      value: " << std::get<int>(token.value) << '\n';
                break;
            case Token::TokenType::SYMBOL:
                std::cout << "SYMBOL         value: " << std::get<char>(token.value) << '\n';
                break;
            case Token::TokenType::STRING_CONST:
                std::cout << "STRING_CONST   value: \"" << std::get<std::string>(token.value) << "\"\n";
                break;

                // --- ������ �κ� ---
            case Token::TokenType::KEYWORD: {
                // 1. variant���� KeywordType enum ���� �����ϴ�.
                auto keywordEnum = std::get<Token::KeywordType>(token.value);
                // 2. enum ���� ���ڿ��� ��ȯ�ϴ� �Լ��� ����մϴ�.
                std::cout << "KEYWORD        value: " << keywordToString(keywordEnum) << '\n';
                break;
            }
                // --- ������ �κ� ---
            case Token::TokenType::IDENTIFIER:
                // IDENTIFIER�� ���, variant�� std::string�� ����Ǿ� �����Ƿ� �ٷ� ����մϴ�.
                std::cout << "IDENTIFIER     value: " << std::get<std::string>(token.value) << '\n';
                break;
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}