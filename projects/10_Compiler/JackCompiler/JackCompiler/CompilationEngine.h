#pragma once

#include "Tokenizer.h"
#include <fstream>
#include <string>

class CompilationEngine {
public:
    CompilationEngine(Tokenizer& tokenizer, const std::string& outputFilename);
    void compile();

private:
    std::ofstream ofs;
    Tokenizer& tokenizer;

    int indentationLevel = 0;
    // helpet method
    void printIndent();

    std::string escapeXml(char symbol);

    void process(Token::KeywordType expectedKeyword);
    void process(char expectedSymbol);
    void process(Token::TokenType expectedType);
    // compile class
    void compileClass();

    // compile class ���� �� �����ƾ
    void compileClassVarDec();
    void compileSubroutine();

    // compile �����ƾ ��� 
    void compileParameterList();
    void compileVarDec();

    // compile statements
    void compileStatements();

    // compile statements ���
    void compileLet();
    void compileIf();
    void compileWhile();
    void compileDo();
    void compileReturn();

    // compile expression
    void compileExpression();

    // compile expression ���
    void compileTerm();
    int compileExpressionList();
};