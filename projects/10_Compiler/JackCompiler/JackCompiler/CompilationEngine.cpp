#include "CompilationEngine.h"

//----------------------------------------------------
// constructor
//----------------------------------------------------

CompilationEngine::CompilationEngine(Tokenizer& tokenizer, const std::string& outputFilename)
	:tokenizer(tokenizer),ofs(outputFilename)
{
	if (!ofs.is_open()) {
		throw std::runtime_error("CompilationEngine() : can't open write file : " + outputFilename);
	}
}

//-----------------------------------------------------
// public method
//-----------------------------------------------------

void CompilationEngine::compile()
{
	this->tokenizer.advance();
	this->compileClass();
}

//-----------------------------------------------------
// private helper method
//-----------------------------------------------------

void CompilationEngine::writeIndent()
{
    ofs << std::string(indentationLevel * 2, ' ');
}

//-----------------------------------------------------

std::string CompilationEngine::escapeXml(char symbol) {
    switch (symbol) {
    case '<':  return "&lt;";
    case '>':  return "&gt;";
    case '&':  return "&amp;";
    case '"':  return "&quot;";
    default:   return std::string(1, symbol);
    }
}

//-----------------------------------------------------

void CompilationEngine::process(Token::KeywordType expectedKeyword)
{
    if (tokenizer.tokenType() == Token::TokenType::KEYWORD && tokenizer.keyword() == expectedKeyword) {
        writeIndent();
        ofs << "<keyword> " << keywordToString(expectedKeyword) << " </keyword>\n";
        tokenizer.advance();
    }
    else {
        std::string got = (tokenizer.tokenType() == Token::TokenType::KEYWORD)
            ? keywordToString(tokenizer.keyword())
            : "a non-keyword token";
        throw std::runtime_error("Syntax Error: Expected keyword '" + keywordToString(expectedKeyword) + "', but got '" + got + "'");
    }
}

//-----------------------------------------------------

void CompilationEngine::process(char expectedSymbol) {
    if (tokenizer.tokenType() == Token::TokenType::SYMBOL && tokenizer.symbol() == expectedSymbol) {
        writeIndent();
        ofs << "<symbol> " << escapeXml(expectedSymbol) << " </symbol>\n";
        tokenizer.advance();
    }
    else {
        throw std::runtime_error("Syntax Error: Expected a symbol + " + escapeXml(expectedSymbol) + ". got = " + escapeXml(tokenizer.symbol()));
    }
}

//-----------------------------------------------------

void CompilationEngine::process(Token::TokenType expectedType)
{
    if (tokenizer.tokenType() == expectedType) {
        switch (expectedType) {
        case Token::TokenType::IDENTIFIER:
            writeIndent();
            ofs << "<identifier> " << tokenizer.identifier() << " </identifier>\n";
            break;
        case Token::TokenType::INT_CONST:
            writeIndent();
            ofs << "<integerConstant> " << tokenizer.intVal() << " </integerConstant>\n";
            break;
        case Token::TokenType::STRING_CONST:
            writeIndent();
            ofs << "<stringConstant> " << tokenizer.stringVal() << " </stringConstant>\n";
            break;
        default:
            throw std::logic_error("process(TokenType) called with an unhandled type. got = " + Token::tokenTypeToString(expectedType));
        }
        tokenizer.advance();
    }
    else {
        throw std::runtime_error("Syntax Error: Expected a token of type " + Token::tokenTypeToString(expectedType) +
            ", but found a different type.");
    }
}
//-----------------------------------------------------
// class component compile
//-----------------------------------------------------

/**
 * @brief Ŭ���� ��ü�� �������Ѵ�.
 * @grammar 'class' className '{' classVarDec* subroutineDec* '}'
 */

void CompilationEngine::compileClass()
{
    writeIndent();
    this->indentationLevel++;

    ofs << "<class>\n";
    process(Token::KeywordType::CLASS);
    process(Token::TokenType::IDENTIFIER);
    process('{');
    // TODO : class field, static, subroutine compile
    while (tokenizer.tokenType() == Token::TokenType::KEYWORD &&
        (tokenizer.keyword() == Token::KeywordType::STATIC || tokenizer.keyword() == Token::KeywordType::FIELD))
    {
        compileClassVarDec(); 
    }

    while (tokenizer.tokenType() == Token::TokenType::KEYWORD &&
        (tokenizer.keyword() == Token::KeywordType::CONSTRUCTOR || tokenizer.keyword() == Token::KeywordType::FUNCTION || tokenizer.keyword() == Token::KeywordType::METHOD))
    {
        compileSubroutine(); 
    }

    process('}');

    this->indentationLevel--;
    writeIndent();
    ofs << "</class>\n";
}

//-----------------------------------------------------

/**
 * @brief ����(static) �Ǵ� �ʵ�(field) ���� ������ �������Ѵ�.
 * @grammar ('static' | 'field') type varName (',' varName)* ';'
 */

void CompilationEngine::compileClassVarDec()
{
    writeIndent();
    this->indentationLevel++;

    ofs << "<classVarDec>\n";
    
    // ù ��° ���� ó��
    process(tokenizer.keyword());

    if (tokenizer.tokenType() == Token::TokenType::KEYWORD) {
        // Ÿ���� 'int', 'char', 'boolean' ���� Ű������ ���
        process(tokenizer.keyword());
    }
    else if (tokenizer.tokenType() == Token::TokenType::IDENTIFIER) {
        // Ÿ���� 'SquareGame' ���� Ŭ���� �̸�(�ĺ���)�� ���
        process(Token::TokenType::IDENTIFIER);
    }
    else {
        throw std::runtime_error("Syntax Error: Expected a type (int, char, boolean, or className).");
    }

    process(Token::TokenType::IDENTIFIER);

    while (tokenizer.tokenType() == Token::TokenType::SYMBOL &&
        tokenizer.symbol() == ',') {
        process(',');
        process(Token::TokenType::IDENTIFIER);
    }

    process(';');

    this->indentationLevel--;
    writeIndent();
    ofs << "</classVarDec>\n";
}

//-----------------------------------------------------

/**
 * @brief �ϳ��� ������ �����ƾ(�޼���, �Լ�, ������)�� �������Ѵ�.
 * @grammar ('constructor'|'function'|'method') ('void'|type) subroutineName '(' parameterList ')' subroutineBody
 */

void CompilationEngine::compileSubroutine()
{
    writeIndent();
    this->indentationLevel++;
    ofs << "<subroutineDec>\n";
    
    process(tokenizer.keyword());
    
    if (tokenizer.tokenType() == Token::TokenType::KEYWORD) {
        // int, boolean �� ������
        process(tokenizer.keyword());
    }
    else {
        process(Token::TokenType::IDENTIFIER);
    }

    process(Token::TokenType::IDENTIFIER);
    process('(');
    compileParameterList();
    process(')');

    compileSubroutineBody();

    this->indentationLevel--;
    writeIndent();
    ofs << "</subroutineDec>\n";
}

//-----------------------------------------------------

/**
 * @brief �Ű� ���� ����� �������Ѵ�. ��ȣ ( �� ) �� ���� ��ū���� ó������ �ʴ´�.
 * @grammar ((type varName) (',' type varName)*)?
 */

void CompilationEngine::compileParameterList()
{
    writeIndent();
    this->indentationLevel++;
    ofs << "<parameterList>\n";

    if (tokenizer.tokenType()!=Token::TokenType::SYMBOL||
        tokenizer.symbol()!=')') {

        if (tokenizer.tokenType() == Token::TokenType::KEYWORD) {
            process(tokenizer.keyword());
        }
        else {
            process(Token::TokenType::IDENTIFIER);
        }

        process(Token::TokenType::IDENTIFIER);

        while (tokenizer.tokenType() == Token::TokenType::SYMBOL && tokenizer.symbol() == ',') {
            process(',');

            if (tokenizer.tokenType() == Token::TokenType::KEYWORD) {
                process(tokenizer.keyword());
            }
            else {
                process(Token::TokenType::IDENTIFIER);
            }

            process(Token::TokenType::IDENTIFIER);
        }
    }

    this->indentationLevel--;
    writeIndent();
    ofs << "</parameterList>\n";
}

//-----------------------------------------------------

void CompilationEngine::compileSubroutineBody()
{
    process('{');
    process('}');
}
