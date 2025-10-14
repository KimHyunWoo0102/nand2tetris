#pragma once
#include<unordered_map>
#include<string>

namespace Token {
	enum class TokenType { 
		KEYWORD, SYMBOL, IDENTIFIER, INT_CONST, STRING_CONST 
	};

    enum class KeywordType {
        // Ŭ����, �����ƾ
        CLASS, CONSTRUCTOR, FUNCTION, METHOD,
        // ���� ����
        FIELD, STATIC, VAR,
        // ���� Ÿ��
        INT, CHAR, BOOLEAN, VOID,
        // ��� Ű����
        TRUE, FALSE, NULL_KEYWORD, THIS,
        // ����
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