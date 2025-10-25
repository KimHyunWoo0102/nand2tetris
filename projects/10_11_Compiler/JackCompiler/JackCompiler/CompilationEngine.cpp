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

Segment CompilationEngine::segmentOf(Kind kind)
{
    switch (kind) {
    case Kind::STATIC: return Segment::STATIC;
    case Kind::FIELD:  return Segment::THIS;   // field�� this ���׸�Ʈ
    case Kind::ARG:    return Segment::ARGUMENT;
    case Kind::VAR:    return Segment::LOCAL;    // var�� local ���׸�Ʈ
    default:           throw std::logic_error("Cannot map NONE kind to segment");
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
    else {
        // return �� ��� ������ ���� ���� ���� ȣ���ڰ� ó���� �����ϴ� arg 0 ��ġ�� �����Ŵ
        // ó���� �ϰ����� ���ؼ� return; �� ��쿡�� 0�� ����
        this->vmWriter.writePush(Segment::CONSTANT, 0);
    }
    process(';');

    this->vmWriter.writeReturn();
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
        char op = tokenizer.symbol();

        process(tokenizer.symbol());
        compileTerm();
        
        switch (op) {
        case '+': vmWriter.writeArithmetic(Command::ADD); break;
        case '-': vmWriter.writeArithmetic(Command::SUB); break;
        case '*': vmWriter.writeCall("Math.multiply", 2); break; 
        case '/': vmWriter.writeCall("Math.divide", 2);   break; 
        case '&': vmWriter.writeArithmetic(Command::AND); break;
        case '|': vmWriter.writeArithmetic(Command::OR);  break;
        case '<': vmWriter.writeArithmetic(Command::LT);  break;
        case '>': vmWriter.writeArithmetic(Command::GT);  break;
        case '=': vmWriter.writeArithmetic(Command::EQ);  break;
        default: throw std::logic_error("Internal Error: Unhandled operator...");
        }
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
        int val = tokenizer.intVal();
        process(Token::TokenType::INT_CONST);

        this->vmWriter.writePush(Segment::CONSTANT, val);
    }
    // Case 2: ���ڿ� ���
    else if (currentToken.type == Token::TokenType::STRING_CONST) {
        std::string strVal = tokenizer.stringVal();
        process(Token::TokenType::STRING_CONST);

        vmWriter.writePush(Segment::CONSTANT, strVal.size()); // .size()�� ���� push
        vmWriter.writeCall("String.new", 1);

        for (char c : strVal) {
            // str[i] (��, c)�� int�� ĳ�����ؼ� push
            vmWriter.writePush(Segment::CONSTANT, static_cast<int>(c));
            vmWriter.writeCall("String.appendChar", 2);
        }
    }
    // Case 3: Ű���� ��� (true,flase,null,this)
    else if (currentToken.type == Token::TokenType::KEYWORD) {
        auto kw = std::get<Token::KeywordType>(currentToken.value);
        process(kw);

        switch (kw)
        {
        case Token::KeywordType::TRUE:
            vmWriter.writePush(Segment::CONSTANT, 0);
            vmWriter.writeArithmetic(Command::NOT);
            break;
        case Token::KeywordType::FALSE:
        case Token::KeywordType::NULL_KEYWORD:
            vmWriter.writePush(Segment::CONSTANT, 0);
            break;
        case Token::KeywordType::THIS:
            vmWriter.writePush(Segment::POINTER, 0);
            break;
        default:
            throw std::runtime_error("Syntax Error: Invalid keyword used as a term.");
            break;
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
        char op = std::get<char>(currentToken.value);
        process(std::get<char>(currentToken.value)); // '-' �Ǵ� '~' ó��
        compileTerm(); // �ڵ����� term ��� ó��

        if (op == '-') {
            this->vmWriter.writeArithmetic(Command::NEG);
        }
        else if (op == '~') {
            this->vmWriter.writeArithmetic(Command::NOT);
        }
    }
    // Case 6: �ĺ��ڷ� �����ϴ� ��� (�迭, ȣ��, ����)
    else if (currentToken.type == Token::TokenType::IDENTIFIER) {
        // peek ����� ���� �б�
        // Case 6-1 : varName '[' expression ']'
        if (nextToken.type == Token::TokenType::SYMBOL && std::get<char>(nextToken.value) == '[') {
            
            auto varName = tokenizer.identifier();
            process(Token::TokenType::IDENTIFIER);

            // if a[expression] = ...
            // push a
            Kind kind = symbolTable.kindOf(varName);
            int index = symbolTable.indexOf(varName);
            vmWriter.writePush(segmentOf(kind), index);

            // compile expression ���� ���ÿ� ������� �������
            process('[');
            compileExpression();
            process(']');
            
            this->vmWriter.writeArithmetic(Command::ADD);

            this->vmWriter.writePop(Segment::POINTER, 1);
            this->vmWriter.writePush(Segment::THAT, 0);
        }
        // Case 6-2 : (className | varName) '.' subroutineName '(' expressionList ')'
        else if (nextToken.type == Token::TokenType::SYMBOL && std::get<char>(nextToken.value) == '.') {
            process(Token::TokenType::IDENTIFIER); // className or varName
            process('.');
            process(Token::TokenType::IDENTIFIER); // subroutineName
            process('(');
            compileExpressionList();
            process(')');
        }
        // Case 6-3 : subroutineName '(' expressionList ')'
        else if (nextToken.type == Token::TokenType::SYMBOL && std::get<char>(nextToken.value) == '(') {
            process(Token::TokenType::IDENTIFIER); // subroutineName
            process('(');
            compileExpressionList();
            process(')');
        }
        // Case 6-4 : varName
        else {
            auto varName = tokenizer.identifier();

            auto index = this->symbolTable.indexOf(varName);
            auto kind = this->symbolTable.kindOf(varName);
            auto segment = this->segmentOf(kind);

            process(Token::TokenType::IDENTIFIER);
            
            this->vmWriter.writePush(segment, index);
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
