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
    // 1. 프로그램 구조 및 클래스 레벨 컴파일
    //    (클래스의 전체 구조와 멤버 변수, 서브루틴 선언을 처리하는 메서드)
    //======================================================================

    void compileClass();

    // ('static' | 'field') type varName (',' varName)* ';'
    void compileClassVarDec();

    /**  ('constructor' | 'function' | 'method') ('void' | type) subroutineName
     * '(' parameterList ')' subroutineBody */
    void compileSubroutine();

    //======================================================================
    // 2. 서브루틴 내부 컴파일
    //    (서브루틴 내부의 파라미터, 지역 변수, 명령어들을 처리하는 메서드)
    //======================================================================

    /** 비어 있을 수 있는 파라미터 목록을 컴파일한다. 양 끝의 "()"는 포함하지 않는다.
     * 문법: ((type varName) (',' type varName)*)? */
    void compileParameterList();

    /** 지역 변수 선언('var' 선언)을 컴파일한다.
     * 문법: 'var' type varName (',' varName)* ';' */
    void compileVarDec();

    /** 연속된 명령어들(statements)을 컴파일한다. 양 끝의 "{}"는 포함하지 않는다. */
    void compileStatements();

    //======================================================================
    // 3. 명령어 컴파일
    //    (let, if, while, do, return 등의 개별 명령어를 처리하는 메서드)
    //======================================================================

    /** 'let' 명령어를 컴파일한다.
     * 문법: 'let' varName ('[' expression ']')? '=' expression ';' */
    void compileLet();

    /** 'if' 명령어를 컴파일한다. 'else' 구문이 있을 수도 있다.
     * 문법: 'if' '(' expression ')' '{' statements '}' ('else' '{' statements '}')? */
    void compileIf();

    /** 'while' 명령어를 컴파일한다.
     * 문법: 'while' '(' expression ')' '{' statements '}' */
    void compileWhile();

    /** 'do' 명령어 (서브루틴 호출)를 컴파일한다.
     * 문법: 'do' subroutineCall ';' */
    void compileDo();

    /** 'return' 명령어를 컴파일한다.
     * 문법: 'return' expression? ';' */
    void compileReturn();

    //======================================================================
    // 4. 표현식 컴파일
    //    (산술/논리 표현식, 항, 서브루틴 호출 등 값과 관련된 부분을 처리하는 메서드)
    //======================================================================

    /** 표현식(expression)을 컴파일한다.
     * 문법: term (op term)* */
    void compileExpression();

    /** 항(term)을 컴파일한다. 이 메서드는 표현식 파서의 핵심이다.
     * 상수, 변수, 배열 접근, 서브루틴 호출, 괄호 표현식 등을 처리한다. */
    void compileTerm();

    /** 쉼표로 구분된 표현식 목록을 컴파일한다. 비어 있을 수도 있다.
     * 서브루틴 호출 시 사용되며, 목록에 있는 표현식의 개수를 반환한다. */
    int compileExpressionList();
};