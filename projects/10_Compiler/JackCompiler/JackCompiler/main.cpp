#include"Tokenizer.h"

std::string keywordToString(Token::KeywordType kw) {
    switch (kw) {
        // 클래스, 서브루틴 관련
    case Token::KeywordType::CLASS:       return "class";
    case Token::KeywordType::METHOD:      return "method";
    case Token::KeywordType::FUNCTION:    return "function";
    case Token::KeywordType::CONSTRUCTOR: return "constructor";

        // 원시 타입
    case Token::KeywordType::INT:         return "int";
    case Token::KeywordType::BOOLEAN:     return "boolean";
    case Token::KeywordType::CHAR:        return "char";
    case Token::KeywordType::VOID:        return "void";

        // 변수 선언
    case Token::KeywordType::VAR:         return "var";
    case Token::KeywordType::STATIC:      return "static";
    case Token::KeywordType::FIELD:       return "field";

        // 문장
    case Token::KeywordType::LET:         return "let";
    case Token::KeywordType::DO:          return "do";
    case Token::KeywordType::IF:          return "if";
    case Token::KeywordType::ELSE:        return "else";
    case Token::KeywordType::WHILE:       return "while";
    case Token::KeywordType::RETURN:      return "return";

        // 상수 키워드
    case Token::KeywordType::TRUE:        return "true";
    case Token::KeywordType::FALSE:       return "false";
    case Token::KeywordType ::NULL_KEYWORD:       return "null";
    case Token::KeywordType::THIS:        return "this";

        // 예외 처리
    default:                          return "Unknown Keyword";
    }
}

#include <iostream>
#include "Tokenizer.h" // Tokenizer와 keywordToString 함수가 포함된 헤더

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

                // --- 수정된 부분 ---
            case Token::TokenType::KEYWORD: {
                // 1. variant에서 KeywordType enum 값을 꺼냅니다.
                auto keywordEnum = std::get<Token::KeywordType>(token.value);
                // 2. enum 값을 문자열로 변환하는 함수를 사용합니다.
                std::cout << "KEYWORD        value: " << keywordToString(keywordEnum) << '\n';
                break;
            }
                // --- 수정된 부분 ---
            case Token::TokenType::IDENTIFIER:
                // IDENTIFIER의 경우, variant에 std::string이 저장되어 있으므로 바로 사용합니다.
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