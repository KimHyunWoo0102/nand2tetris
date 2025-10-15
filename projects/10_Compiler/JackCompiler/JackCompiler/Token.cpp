#include "Token.h"

//---------------------------------------------------
// helper method
//---------------------------------------------------

std::string Token::keywordToString(KeywordType kw)
{
    switch (kw) {
        // 클래스, 서브루틴 관련
    case KeywordType::CLASS:       return "class";
    case KeywordType::METHOD:      return "method";
    case KeywordType::FUNCTION:    return "function";
    case KeywordType::CONSTRUCTOR: return "constructor";

        // 원시 타입
    case KeywordType::INT:         return "int";
    case KeywordType::BOOLEAN:     return "boolean";
    case KeywordType::CHAR:        return "char";
    case KeywordType::VOID:        return "void";

        // 변수 선언
    case KeywordType::VAR:         return "var";
    case KeywordType::STATIC:      return "static";
    case KeywordType::FIELD:       return "field";

        // 문장
    case KeywordType::LET:         return "let";
    case KeywordType::DO:          return "do";
    case KeywordType::IF:          return "if";
    case KeywordType::ELSE:        return "else";
    case KeywordType::WHILE:       return "while";
    case KeywordType::RETURN:      return "return";

        // 상수 키워드
    case KeywordType::TRUE:        return "true";
    case KeywordType::FALSE:       return "false";
    case KeywordType::NULL_KEYWORD:       return "null";
    case KeywordType::THIS:        return "this";

        // 예외 처리
    default:                          return "Unknown Keyword";
    }
}

//---------------------------------------------------

std::string Token::tokenTypeToString(TokenType type)
{
    switch (type) {
    case TokenType::KEYWORD:       return "KEYWORD";
    case TokenType::SYMBOL:        return "SYMBOL";
    case TokenType::IDENTIFIER:    return "IDENTIFIER";
    case TokenType::INT_CONST:     return "INT_CONST";
    case TokenType::STRING_CONST:  return "STRING_CONST";
    default:                       return "Unknown Token Type";
    }
}
