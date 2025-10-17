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
 * @brief 클래스 전체를 컴파일한다.
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
 * @brief 정적(static) 또는 필드(field) 변수 선언을 컴파일한다.
 * @grammar ('static' | 'field') type varName (',' varName)* ';'
 */

void CompilationEngine::compileClassVarDec()
{
    writeIndent();
    this->indentationLevel++;

    ofs << "<classVarDec>\n";
    
    // 첫 번째 변수 처리
    process(tokenizer.keyword());

    if (tokenizer.tokenType() == Token::TokenType::KEYWORD) {
        // 타입이 'int', 'char', 'boolean' 같은 키워드인 경우
        process(tokenizer.keyword());
    }
    else if (tokenizer.tokenType() == Token::TokenType::IDENTIFIER) {
        // 타입이 'SquareGame' 같은 클래스 이름(식별자)인 경우
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
 * @brief 하나의 완전한 서브루틴(메서드, 함수, 생성자)을 컴파일한다.
 * @grammar ('constructor'|'function'|'method') ('void'|type) subroutineName '(' parameterList ')' subroutineBody
 */

void CompilationEngine::compileSubroutine()
{
    writeIndent();
    this->indentationLevel++;
    ofs << "<subroutineDec>\n";
    
    process(tokenizer.keyword());
    
    if (tokenizer.tokenType() == Token::TokenType::KEYWORD) {
        // int, boolean 등 내장형
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
 * @brief 매개 변수 목록을 컴파일한다. 괄호 ( 와 ) 로 감싼 토큰들은 처리하지 않는다.
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
// subroutine component compile method
//-----------------------------------------------------

/**
 * @brief 서브루틴의 몸체('{...}')를 컴파일한다.
 * @grammar '{' varDec* statements '}'
 */

void CompilationEngine::compileSubroutineBody()
{
    writeIndent();
    this->indentationLevel++;
    ofs << "<subroutineBody>\n";

    process('{');

    while (tokenizer.tokenType() == Token::TokenType::KEYWORD && tokenizer.keyword() == Token::KeywordType::VAR) {
        compileVarDec();
    }
    compileStatements();

    process('}');

    this->indentationLevel--;
    writeIndent();
    ofs << "</subroutineBody>\n";
}

//-----------------------------------------------------

/**
 * @brief 지역 변수 선언('var' 선언)을 컴파일한다.
 * @grammar 'var' type varName (',' varName)* ';'
 */

void CompilationEngine::compileVarDec()
{
    writeIndent();
    this->indentationLevel++;
    ofs << "<varDec>\n";

    process(Token::KeywordType::VAR);
    
    if (tokenizer.tokenType()==Token::TokenType::KEYWORD) {
        process(tokenizer.keyword());
    }
    else {
        process(Token::TokenType::IDENTIFIER);
    }

    process(Token::TokenType::IDENTIFIER);

    while (tokenizer.tokenType() == Token::TokenType::SYMBOL && tokenizer.symbol() == ',') {
        process(',');
        process(Token::TokenType::IDENTIFIER);
    }

    process(';');
    
    this->indentationLevel--;
    writeIndent();
    ofs << "</varDec>\n";
}

//-----------------------------------------------------

/**
 * @brief 연속된 명령어들(statements)을 컴파일한다. 양 끝의 "{}"는 포함하지 않는다.
 * @grammar statement*
 */
void CompilationEngine::compileStatements()
{
    writeIndent();
    ofs << "<statements>\n";
    indentationLevel++;

    while (tokenizer.hasMoreTokens()) {
        const auto& nextToken = tokenizer.peekToken();

        if (nextToken.type != Token::TokenType::KEYWORD)
            break;

        auto kw = std::get<Token::KeywordType>(nextToken.value);

        switch (kw) {
            case Token::KeywordType::LET:    compileLet();    break;
            case Token::KeywordType::IF:     compileIf();     break;
            case Token::KeywordType::WHILE:  compileWhile();  break;
            case Token::KeywordType::DO:     compileDo();     break;
            case Token::KeywordType::RETURN: compileReturn(); break;
            default: goto end_loop;
        }
    }

end_loop:
    indentationLevel--;
    writeIndent();
    ofs << "</statements>\n";
}

//-----------------------------------------------------
// Statements component compile method
//-----------------------------------------------------

/**
 * @brief 'let' 명령어를 컴파일한다.
 * @grammar 'let' varName ('[' expression ']')? '=' expression ';'
 */

void CompilationEngine::compileLet()
{
    // ...
}

//-----------------------------------------------------

/**
 * @brief 'if' 명령어를 컴파일한다. 'else' 구문이 있을 수도 있다.
 * @grammar 'if' '(' expression ')' '{' statements '}' ('else' '{' statements '}')?
 */

void CompilationEngine::compileIf()
{
    // ...
}

//-----------------------------------------------------

/**
 * @brief 'while' 명령어를 컴파일한다.
 * @grammar 'while' '(' expression ')' '{' statements '}'
 */

void CompilationEngine::compileWhile()
{
    // ...
}

//-----------------------------------------------------

/**
 * @brief 'do' 명령어 (서브루틴 호출)를 컴파일한다.
 * @grammar 'do' subroutineCall ';'
 */

void CompilationEngine::compileDo()
{
    // ...
}

//-----------------------------------------------------

/**
 * @brief 'return' 명령어를 컴파일한다.
 * @grammar 'return' expression? ';'
 */

void CompilationEngine::compileReturn()
{
    // ...
}

//-----------------------------------------------------
