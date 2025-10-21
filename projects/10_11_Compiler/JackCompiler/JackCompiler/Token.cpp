#include "Token.h"

//---------------------------------------------------
// helper method
//---------------------------------------------------

std::string Token::keywordToString(KeywordType kw)
{
    switch (kw) {
        // Ŭ����, �����ƾ ����
    case KeywordType::CLASS:       return "class";
    case KeywordType::METHOD:      return "method";
    case KeywordType::FUNCTION:    return "function";
    case KeywordType::CONSTRUCTOR: return "constructor";

        // ���� Ÿ��
    case KeywordType::INT:         return "int";
    case KeywordType::BOOLEAN:     return "boolean";
    case KeywordType::CHAR:        return "char";
    case KeywordType::VOID:        return "void";

        // ���� ����
    case KeywordType::VAR:         return "var";
    case KeywordType::STATIC:      return "static";
    case KeywordType::FIELD:       return "field";

        // ����
    case KeywordType::LET:         return "let";
    case KeywordType::DO:          return "do";
    case KeywordType::IF:          return "if";
    case KeywordType::ELSE:        return "else";
    case KeywordType::WHILE:       return "while";
    case KeywordType::RETURN:      return "return";

        // ��� Ű����
    case KeywordType::TRUE:        return "true";
    case KeywordType::FALSE:       return "false";
    case KeywordType::NULL_KEYWORD:       return "null";
    case KeywordType::THIS:        return "this";

        // ���� ó��
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
