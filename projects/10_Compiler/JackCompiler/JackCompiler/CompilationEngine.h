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

    // compile class 변수 및 서브루틴
    void compileClassVarDec();
    void compileSubroutine();

    // compile 서브루틴 요소 
    void compileParameterList();
    void compileVarDec();

    // compile statements
    void compileStatements();

    // compile statements 요소
    void compileLet();
    void compileIf();
    void compileWhile();
    void compileDo();
    void compileReturn();

    // compile expression
    void compileExpression();

    // compile expression 요소
    void compileTerm();
    int compileExpressionList();
};