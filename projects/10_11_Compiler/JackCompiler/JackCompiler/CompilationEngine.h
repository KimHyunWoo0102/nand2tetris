#pragma once

#include"VMWriter.h"
#include"SymbolTable.h"
#include "Tokenizer.h"
#include <fstream>
#include <string>

class CompilationEngine {
public:
    CompilationEngine(Tokenizer& tokenizer,const std::string& className, const std::string& outputFilePath);
    void compile();

private:
    Tokenizer& tokenizer;
    VMWriter vmWriter;
    SymbolTable symbolTable;
    std::string className;
    int labelCounter = 0;

    void process(Token::KeywordType expectedKeyword);
    void process(char expectedSymbol);
    void process(Token::TokenType expectedType);

    //helper Method
    bool isOperator(char c);
    Segment segmentOf(Kind kind);
    std::string generateLabel(const std::string& prefix) {
        auto label = prefix + std::to_string(labelCounter);
        labelCounter++;
        return label;
    }
    // compile class
    void compileClass();

    // compile class 변수 및 서브루틴
    void compileClassVarDec();
    void compileSubroutine();

    // compile 서브루틴 요소 
    void compileParameterList();
    void compileSubroutineBody();
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