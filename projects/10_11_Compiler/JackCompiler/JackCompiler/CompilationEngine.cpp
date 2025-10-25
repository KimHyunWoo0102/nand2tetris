#include "CompilationEngine.h"

//----------------------------------------------------
// constructor
//----------------------------------------------------

CompilationEngine::CompilationEngine(Tokenizer& tokenizer, const std::string& className, const std::string& outputFilePath)
    :tokenizer(tokenizer),className(className),vmWriter(outputFilePath)
{
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

void CompilationEngine::process(Token::KeywordType expectedKeyword)
{
    if (tokenizer.tokenType() == Token::TokenType::KEYWORD && tokenizer.keyword() == expectedKeyword) {
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
        tokenizer.advance();
    }
    else {
        throw std::runtime_error("Syntax Error: ");
    }
}

//-----------------------------------------------------

void CompilationEngine::process(Token::TokenType expectedType)
{
    if (tokenizer.tokenType() == expectedType) {
        switch (expectedType) {
        case Token::TokenType::IDENTIFIER:
            break;
        case Token::TokenType::INT_CONST:
            break;
        case Token::TokenType::STRING_CONST:
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
 * @brief Ŭ���� ��ü�� �������Ѵ�.
 * @grammar 'class' className '{' classVarDec* subroutineDec* '}'
 */

void CompilationEngine::compileClass()
{
    process(Token::KeywordType::CLASS);
    process(Token::TokenType::IDENTIFIER);
    process('{');

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
}

//-----------------------------------------------------

/**
 * @brief ����(static) �Ǵ� �ʵ�(field) ���� ������ �������Ѵ�.
 * @grammar ('static' | 'field') type varName (',' varName)* ';'
 */

void CompilationEngine::compileClassVarDec()
{
    Kind kind;
    std::string type;
    std::string name;

    if (tokenizer.keyword() == Token::KeywordType::STATIC) {
        kind = Kind::STATIC;
    }
    else {
        kind = Kind::FIELD;
    }

    // ù ��° ���� ó��
    process(tokenizer.keyword());

    if (tokenizer.tokenType() == Token::TokenType::KEYWORD) {
        type = keywordToString(tokenizer.keyword()); 
        process(tokenizer.keyword());
    }
    else if (tokenizer.tokenType() == Token::TokenType::IDENTIFIER) {
        type = tokenizer.identifier(); 
        process(Token::TokenType::IDENTIFIER);
    }
    else {
        throw std::runtime_error("Syntax Error: Expected a type.");
    }

    name = tokenizer.identifier();
    process(Token::TokenType::IDENTIFIER);
    symbolTable.define(name, type, kind);

    while (tokenizer.tokenType() == Token::TokenType::SYMBOL &&
        tokenizer.symbol() == ',') {
        process(','); // ��ǥ �Һ�
        name = tokenizer.identifier(); // ���� �̸� ����
        process(Token::TokenType::IDENTIFIER); // ���� �̸� �Һ�
        symbolTable.define(name, type, kind);
    }

    process(';');

    //for test
    //this->symbolTable.printTable();
}

//-----------------------------------------------------

/**
 * @brief �ϳ��� ������ �����ƾ(�޼���, �Լ�, ������)�� �������Ѵ�.
 * @grammar ('constructor'|'function'|'method') ('void'|type) subroutineName '(' parameterList ')' subroutineBody
 */

void CompilationEngine::compileSubroutine()
{
    this->symbolTable.resetTable();

    if (tokenizer.keyword() == Token::KeywordType::METHOD) {
        this->symbolTable.define("this", this->className, Kind::ARG);
    }

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

    this->symbolTable.printTable();
}

//-----------------------------------------------------

/**
 * @brief �Ű� ���� ����� �������Ѵ�. ��ȣ ( �� ) �� ���� ��ū���� ó������ �ʴ´�.
 * @grammar ((type varName) (',' type varName)*)?
 */

void CompilationEngine::compileParameterList()
{
    Kind kind = Kind::ARG;
    std::string type;
    std::string name;

    if (!(tokenizer.tokenType() == Token::TokenType::SYMBOL && tokenizer.symbol() == ')')) {

        if (tokenizer.tokenType() == Token::TokenType::KEYWORD) { 
            type = keywordToString(tokenizer.keyword());
            process(tokenizer.keyword());
        }
        else {
            type = tokenizer.identifier();
            process(Token::TokenType::IDENTIFIER);
        }

        name = tokenizer.identifier();
        process(Token::TokenType::IDENTIFIER);


        symbolTable.define(name, type, kind);

        while (tokenizer.tokenType() == Token::TokenType::SYMBOL && tokenizer.symbol() == ',') {
            process(',');

            if (tokenizer.tokenType() == Token::TokenType::KEYWORD) {
                type = keywordToString(tokenizer.keyword());
                process(tokenizer.keyword());
            }
            else {
                type = tokenizer.identifier();
                process(Token::TokenType::IDENTIFIER);
            }
            name = tokenizer.identifier();
            process(Token::TokenType::IDENTIFIER);

            symbolTable.define(name, type, kind);
        }
    }
}


//-----------------------------------------------------
// subroutine component compile method
//-----------------------------------------------------

/**
 * @brief �����ƾ�� ��ü('{...}')�� �������Ѵ�.
 * @grammar '{' varDec* statements '}'
 */

void CompilationEngine::compileSubroutineBody()
{
    process('{');

    while (tokenizer.tokenType() == Token::TokenType::KEYWORD && tokenizer.keyword() == Token::KeywordType::VAR) {
        compileVarDec();
    }
    compileStatements();

    process('}');
}

//-----------------------------------------------------

/**
 * @brief ���� ���� ����('var' ����)�� �������Ѵ�.
 * @grammar 'var' type varName (',' varName)* ';'
 */

void CompilationEngine::compileVarDec()
{
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
}

//-----------------------------------------------------

/**
 * @brief ���ӵ� ��ɾ��(statements)�� �������Ѵ�. �� ���� "{}"�� �������� �ʴ´�.
 * @grammar statement*
 */

void CompilationEngine::compileStatements()
{
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
    ;
}

//-----------------------------------------------------
// Statements component compile method
//-----------------------------------------------------

/**
 * @brief 'let' ��ɾ �������Ѵ�.
 * @grammar 'let' varName ('[' expression ']')? '=' expression ';'
 */

void CompilationEngine::compileLet()
{
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
}

//-----------------------------------------------------

/**
 * @brief 'if' ��ɾ �������Ѵ�. 'else' ������ ���� ���� �ִ�.
 * @grammar 'if' '(' expression ')' '{' statements '}' ('else' '{' statements '}')?
 */

void CompilationEngine::compileIf()
{
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
}

//-----------------------------------------------------

/**
 * @brief 'while' ��ɾ �������Ѵ�.
 * @grammar 'while' '(' expression ')' '{' statements '}'
 */

void CompilationEngine::compileWhile()
{
    process(Token::KeywordType::WHILE);
    process('(');
    compileExpression();
    process(')');

    process('{');
    compileStatements();
    process('}');
}

//-----------------------------------------------------

/**
 * @brief 'do' ��ɾ� (�����ƾ ȣ��)�� �������Ѵ�.
 * @grammar 'do' subroutineCall ';'
 */

void CompilationEngine::compileDo()
{
    process(Token::KeywordType::DO);
    compileTerm();
    process(';');
}

//-----------------------------------------------------

/**
 * @brief 'return' ��ɾ �������Ѵ�.
 * @grammar 'return' expression? ';'
 */

void CompilationEngine::compileReturn()
{
    process(Token::KeywordType::RETURN);

    if (!(tokenizer.tokenType() == Token::TokenType::SYMBOL && tokenizer.symbol() == ';')) {
        compileExpression();
    }

    process(';');
}

//-----------------------------------------------------

/**
 * @brief ǥ����(expression)�� �������Ѵ�. 
 * @grammar term(op term)*
 */

void CompilationEngine::compileExpression()
{
    compileTerm();

    while (tokenizer.tokenType() == Token::TokenType::SYMBOL&&isOperator(tokenizer.symbol())) {
        process(tokenizer.symbol());
        compileTerm();
    }
}

//-----------------------------------------------------

/**
 * @brief ��(term)�� �������Ѵ�. ǥ���� �ļ��� �ٽ��̴�.
 * @grammar integerConstant | stringConstant | keywordConstant | varName | varName '[' expression ']' | subroutineCall | '(' expression ')' | unaryOp term
 */

void CompilationEngine::compileTerm() {
    const auto& currentToken = tokenizer.getCurrentToken();
    const auto& nextToken = tokenizer.peekToken();

    // Case 1: ���� ���
    if (currentToken.type == Token::TokenType::INT_CONST) {
        process(Token::TokenType::INT_CONST);
    }
    // Case 2: ���ڿ� ���
    else if (currentToken.type == Token::TokenType::STRING_CONST) {
        process(Token::TokenType::STRING_CONST);
    }
    // Case 3: Ű���� ���
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
    // Case 4: ��ȣ ǥ���� '(' expression ')'
    else if (currentToken.type == Token::TokenType::SYMBOL && std::get<char>(currentToken.value) == '(') {
        process('(');
        compileExpression();
        process(')');
    }
    // Case 5: ���� ������ unaryOp term
    else if (currentToken.type == Token::TokenType::SYMBOL &&
        (std::get<char>(currentToken.value) == '-' || std::get<char>(currentToken.value) == '~'))
    {
        process(std::get<char>(currentToken.value)); // '-' �Ǵ� '~' ó��
        compileTerm(); // �ڵ����� term ��� ó��
    }
    // Case 6: �ĺ��ڷ� �����ϴ� ��� (�迭, ȣ��, ����)
    else if (currentToken.type == Token::TokenType::IDENTIFIER) {
        // peek ����� ���� �б�
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
}

//-----------------------------------------------------

/**
 * @brief ��ǥ�� ���е� (��� ���� �� �ִ�) ǥ���� ����� �������Ѵ�.
 * @grammar (expression (',' expression)*)?
 * @return ��Ͽ� �ִ� ǥ������ ������ ��ȯ�Ѵ�.
 */

int CompilationEngine::compileExpressionList()
{
    int cnt = 0;

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

    return cnt;
}

//-----------------------------------------------------
