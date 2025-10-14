#pragma once
#include<unordered_map>
#include<string>

namespace Token {
	enum class TokenType { 
		KEYWORD, SYMBOL, IDENTIFIER, INT_CONST, STRING_CONST 
	};

    enum class KeywordType {
        // 클래스, 서브루틴
        CLASS, CONSTRUCTOR, FUNCTION, METHOD,
        // 변수 선언
        FIELD, STATIC, VAR,
        // 원시 타입
        INT, CHAR, BOOLEAN, VOID,
        // 상수 키워드
        TRUE, FALSE, NULL_KEYWORD, THIS,
        // 문장
        LET, DO, IF, ELSE, WHILE, RETURN
    };

    const std::unordered_map<std::string, KeywordType> keywordMap = {
         {"class",       KeywordType::CLASS},
         {"constructor", KeywordType::CONSTRUCTOR},
         {"function",    KeywordType::FUNCTION},
         {"method",      KeywordType::METHOD},
         {"field",       KeywordType::FIELD},
         {"static",      KeywordType::STATIC},
         {"var",         KeywordType::VAR},
         {"int",         KeywordType::INT},
         {"char",        KeywordType::CHAR},
         {"boolean",     KeywordType::BOOLEAN},
         {"void",        KeywordType::VOID},
         {"true",        KeywordType::TRUE},
         {"false",       KeywordType::FALSE},
         {"null",        KeywordType::NULL_KEYWORD},
         {"this",        KeywordType::THIS},
         {"let",         KeywordType::LET},
         {"do",          KeywordType::DO},
         {"if",          KeywordType::IF},
         {"else",        KeywordType::ELSE},
         {"while",       KeywordType::WHILE},
         {"return",      KeywordType::RETURN}
    };
}