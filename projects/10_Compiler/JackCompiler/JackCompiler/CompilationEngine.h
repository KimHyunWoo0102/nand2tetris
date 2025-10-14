#pragma once

#include "Tokenizer.h"
#include <fstream>
#include <string>

class CompilationEngine {
public:
    CompilationEngine(Tokenizer& tokenizer, const std::string& outputFilename);
    void compile();

private:
    std::ofstream m_outputFile;
    Tokenizer& m_tokenizer;


    //======================================================================
    // 1. ���α׷� ���� �� Ŭ���� ���� ������
    //    (Ŭ������ ��ü ������ ��� ����, �����ƾ ������ ó���ϴ� �޼���)
    //======================================================================

    void compileClass();

    // ('static' | 'field') type varName (',' varName)* ';'
    void compileClassVarDec();

    /**  ('constructor' | 'function' | 'method') ('void' | type) subroutineName
     * '(' parameterList ')' subroutineBody */
    void compileSubroutine();

    //======================================================================
    // 2. �����ƾ ���� ������
    //    (�����ƾ ������ �Ķ����, ���� ����, ��ɾ���� ó���ϴ� �޼���)
    //======================================================================

    /** ��� ���� �� �ִ� �Ķ���� ����� �������Ѵ�. �� ���� "()"�� �������� �ʴ´�.
     * ����: ((type varName) (',' type varName)*)? */
    void compileParameterList();

    /** ���� ���� ����('var' ����)�� �������Ѵ�.
     * ����: 'var' type varName (',' varName)* ';' */
    void compileVarDec();

    /** ���ӵ� ��ɾ��(statements)�� �������Ѵ�. �� ���� "{}"�� �������� �ʴ´�. */
    void compileStatements();

    //======================================================================
    // 3. ��ɾ� ������
    //    (let, if, while, do, return ���� ���� ��ɾ ó���ϴ� �޼���)
    //======================================================================

    /** 'let' ��ɾ �������Ѵ�.
     * ����: 'let' varName ('[' expression ']')? '=' expression ';' */
    void compileLet();

    /** 'if' ��ɾ �������Ѵ�. 'else' ������ ���� ���� �ִ�.
     * ����: 'if' '(' expression ')' '{' statements '}' ('else' '{' statements '}')? */
    void compileIf();

    /** 'while' ��ɾ �������Ѵ�.
     * ����: 'while' '(' expression ')' '{' statements '}' */
    void compileWhile();

    /** 'do' ��ɾ� (�����ƾ ȣ��)�� �������Ѵ�.
     * ����: 'do' subroutineCall ';' */
    void compileDo();

    /** 'return' ��ɾ �������Ѵ�.
     * ����: 'return' expression? ';' */
    void compileReturn();

    //======================================================================
    // 4. ǥ���� ������
    //    (���/�� ǥ����, ��, �����ƾ ȣ�� �� ���� ���õ� �κ��� ó���ϴ� �޼���)
    //======================================================================

    /** ǥ����(expression)�� �������Ѵ�.
     * ����: term (op term)* */
    void compileExpression();

    /** ��(term)�� �������Ѵ�. �� �޼���� ǥ���� �ļ��� �ٽ��̴�.
     * ���, ����, �迭 ����, �����ƾ ȣ��, ��ȣ ǥ���� ���� ó���Ѵ�. */
    void compileTerm();

    /** ��ǥ�� ���е� ǥ���� ����� �������Ѵ�. ��� ���� ���� �ִ�.
     * �����ƾ ȣ�� �� ���Ǹ�, ��Ͽ� �ִ� ǥ������ ������ ��ȯ�Ѵ�. */
    int compileExpressionList();
};