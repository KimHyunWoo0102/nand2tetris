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
 * @brief �����ƾ�� ��ü('{...}')�� �������Ѵ�.
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
 * @brief ���� ���� ����('var' ����)�� �������Ѵ�.
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
 * @brief ���ӵ� ��ɾ��(statements)�� �������Ѵ�. �� ���� "{}"�� �������� �ʴ´�.
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
 * @brief 'let' ��ɾ �������Ѵ�.
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
 * @brief 'if' ��ɾ �������Ѵ�. 'else' ������ ���� ���� �ִ�.
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
 * @brief 'while' ��ɾ �������Ѵ�.
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
 * @brief 'do' ��ɾ� (�����ƾ ȣ��)�� �������Ѵ�.
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
 * @brief 'return' ��ɾ �������Ѵ�.
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
 * @brief ǥ����(expression)�� �������Ѵ�. 
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
 * @brief ��(term)�� �������Ѵ�. ǥ���� �ļ��� �ٽ��̴�.
 * @grammar integerConstant | stringConstant | keywordConstant | varName | varName '[' expression ']' | subroutineCall | '(' expression ')' | unaryOp term
 */

void CompilationEngine::compileTerm() {
    writeIndent();
    ofs << "<term>\n";
    indentationLevel++;

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

    indentationLevel--;
    writeIndent();
    ofs << "</term>\n";
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
