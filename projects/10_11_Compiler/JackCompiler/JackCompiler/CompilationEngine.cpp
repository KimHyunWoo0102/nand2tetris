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

bool CompilationEngine::isOperator(char symbol) {
    const std::string ops = "+-*/&|<>=";
    return ops.find(symbol) != std::string::npos;
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

    if (!(tokenizer.tokenType() == Token::TokenType::SYMBOL && tokenizer.symbol() == ')')) {

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
        const auto& currentToken = tokenizer.getCurrentToken();

        if (currentToken.type != Token::TokenType::KEYWORD)
            break;

        auto kw = std::get<Token::KeywordType>(currentToken.value);

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
    writeIndent();
    ofs << "<letStatement>\n";
    indentationLevel++;

    process(Token::KeywordType::LET);
    process(Token::TokenType::IDENTIFIER);

    if (tokenizer.tokenType() == Token::TokenType::SYMBOL &&
        tokenizer.symbol() == '[') {
        process('[');
        compileExpression();
        process(']');
    }

    process('=');
    compileExpression();
    process(';');

    indentationLevel--;
    writeIndent();
    ofs << "</letStatement>\n";
}

//-----------------------------------------------------

/**
 * @brief 'if' 명령어를 컴파일한다. 'else' 구문이 있을 수도 있다.
 * @grammar 'if' '(' expression ')' '{' statements '}' ('else' '{' statements '}')?
 */

void CompilationEngine::compileIf()
{
    writeIndent();
    ofs << "<ifStatement>\n";
    indentationLevel++;

    process(Token::KeywordType::IF);
    process('(');
    compileExpression();
    process(')');

    process('{');
    compileStatements();
    process('}');

    if (tokenizer.keyword() == Token::KeywordType::ELSE) {
        process(Token::KeywordType::ELSE);
        process('{');
        compileStatements();
        process('}');
    }


    indentationLevel--;
    writeIndent();
    ofs << "</ifStatement>\n";
}

//-----------------------------------------------------

/**
 * @brief 'while' 명령어를 컴파일한다.
 * @grammar 'while' '(' expression ')' '{' statements '}'
 */

void CompilationEngine::compileWhile()
{
    writeIndent();
    ofs << "<whileStatement>\n";
    indentationLevel++;

    process(Token::KeywordType::WHILE);
    process('(');
    compileExpression();
    process(')');

    process('{');
    compileStatements();
    process('}');

    indentationLevel--;
    writeIndent();
    ofs << "</whileStatement>\n";
}

//-----------------------------------------------------

/**
 * @brief 'do' 명령어 (서브루틴 호출)를 컴파일한다.
 * @grammar 'do' subroutineCall ';'
 */

void CompilationEngine::compileDo()
{
    writeIndent();
    ofs << "<doStatement>\n";
    indentationLevel++;

    process(Token::KeywordType::DO);
    compileTerm();
    process(';');

    indentationLevel--;
    writeIndent();
    ofs << "</doStatement>\n";
}

//-----------------------------------------------------

/**
 * @brief 'return' 명령어를 컴파일한다.
 * @grammar 'return' expression? ';'
 */

void CompilationEngine::compileReturn()
{
    writeIndent();
    ofs << "<returnStatement>\n";
    indentationLevel++;

    process(Token::KeywordType::RETURN);

    if (!(tokenizer.tokenType() == Token::TokenType::SYMBOL && tokenizer.symbol() == ';')) {
        compileExpression();
    }

    process(';');

    indentationLevel--;
    writeIndent();
    ofs << "</returnStatement>\n";
}

//-----------------------------------------------------

/**
 * @brief 표현식(expression)을 컴파일한다. 
 * @grammar term(op term)*
 */

void CompilationEngine::compileExpression()
{
    writeIndent();
    ofs << "<expression>\n";
    indentationLevel++;

    compileTerm();

    while (tokenizer.tokenType() == Token::TokenType::SYMBOL&&isOperator(tokenizer.symbol())) {
        process(tokenizer.symbol());
        compileTerm();
    }

    indentationLevel--;
    writeIndent();
    ofs << "</expression>\n";
}

//-----------------------------------------------------

/**
 * @brief 항(term)을 컴파일한다. 표현식 파서의 핵심이다.
 * @grammar integerConstant | stringConstant | keywordConstant | varName | varName '[' expression ']' | subroutineCall | '(' expression ')' | unaryOp term
 */

void CompilationEngine::compileTerm() {
    writeIndent();
    ofs << "<term>\n";
    indentationLevel++;

    const auto& currentToken = tokenizer.getCurrentToken();
    const auto& nextToken = tokenizer.peekToken();

    // Case 1: 정수 상수
    if (currentToken.type == Token::TokenType::INT_CONST) {
        process(Token::TokenType::INT_CONST);
    }
    // Case 2: 문자열 상수
    else if (currentToken.type == Token::TokenType::STRING_CONST) {
        process(Token::TokenType::STRING_CONST);
    }
    // Case 3: 키워드 상수
    else if (currentToken.type == Token::TokenType::KEYWORD) {
        auto kw = std::get<Token::KeywordType>(currentToken.value);
        if (kw == Token::KeywordType::TRUE || kw == Token::KeywordType::FALSE ||
            kw == Token::KeywordType::NULL_KEYWORD || kw == Token::KeywordType::THIS)
        {
            process(kw);
        }
        else {
            throw std::runtime_error("Syntax Error: Invalid keyword used as a term.");
        }
    }
    // Case 4: 괄호 표현식 '(' expression ')'
    else if (currentToken.type == Token::TokenType::SYMBOL && std::get<char>(currentToken.value) == '(') {
        process('(');
        compileExpression();
        process(')');
    }
    // Case 5: 단항 연산자 unaryOp term
    else if (currentToken.type == Token::TokenType::SYMBOL &&
        (std::get<char>(currentToken.value) == '-' || std::get<char>(currentToken.value) == '~'))
    {
        process(std::get<char>(currentToken.value)); // '-' 또는 '~' 처리
        compileTerm(); // 뒤따르는 term 재귀 처리
    }
    // Case 6: 식별자로 시작하는 경우 (배열, 호출, 변수)
    else if (currentToken.type == Token::TokenType::IDENTIFIER) {
        // peek 결과를 보고 분기
        if (nextToken.type == Token::TokenType::SYMBOL && std::get<char>(nextToken.value) == '[') {
            process(Token::TokenType::IDENTIFIER); // varName
            process('[');
            compileExpression();
            process(']');
        }
        else if (nextToken.type == Token::TokenType::SYMBOL && std::get<char>(nextToken.value) == '.') {
            process(Token::TokenType::IDENTIFIER); // className or varName
            process('.');
            process(Token::TokenType::IDENTIFIER); // subroutineName
            process('(');
            compileExpressionList();
            process(')');
        }
        else if (nextToken.type == Token::TokenType::SYMBOL && std::get<char>(nextToken.value) == '(') {
            process(Token::TokenType::IDENTIFIER); // subroutineName
            process('(');
            compileExpressionList();
            process(')');
        }
        else {
            process(Token::TokenType::IDENTIFIER);
        }
    }
    else {
        //throw std::runtime_error("Syntax Error: Invalid term structure.");
    }

    indentationLevel--;
    writeIndent();
    ofs << "</term>\n";
}

//-----------------------------------------------------

/**
 * @brief 쉼표로 구분된 (비어 있을 수 있는) 표현식 목록을 컴파일한다.
 * @grammar (expression (',' expression)*)?
 * @return 목록에 있는 표현식의 개수를 반환한다.
 */

int CompilationEngine::compileExpressionList()
{
    int cnt = 0;

    writeIndent();
    ofs << "<expressionList>\n";
    indentationLevel++;

    if (!(tokenizer.tokenType() == Token::TokenType::SYMBOL && tokenizer.symbol() == ')'))
    {
        compileExpression();
        ++cnt;

        while (tokenizer.tokenType() == Token::TokenType::SYMBOL && tokenizer.symbol() == ',') {
            process(',');
            compileExpression();
            ++cnt;
        }
    }

    indentationLevel--;
    writeIndent();
    ofs << "</expressionList>\n";
    return cnt;
}

//-----------------------------------------------------
