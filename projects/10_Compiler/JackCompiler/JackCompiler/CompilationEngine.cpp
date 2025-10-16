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
        // 이제 내부 헬퍼 함수를 호출한다.
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
            ofs << "<identifier> " << tokenizer.identifier() << " </identifier>\n";
            break;
        case Token::TokenType::INT_CONST:
            ofs << "<integerConstant> " << tokenizer.intVal() << " </integerConstant>\n";
            break;
        case Token::TokenType::STRING_CONST:
            ofs << "<stringConstant> " << tokenizer.stringVal() << " </stringConstant>\n";
            break;
        default:
            throw std::logic_error("process(TokenType) called with an unhandled type.");
        }
        tokenizer.advance();
    }
    else {
        throw std::runtime_error("Syntax Error: Expected a token of type " + Token::tokenTypeToString(expectedType) +
            ", but found a different type.");
    }
}
//-----------------------------------------------------

void CompilationEngine::compileClass()
{
    ofs << "<class>\n";
    process(Token::KeywordType::CLASS);
    process(Token::TokenType::IDENTIFIER);
    process('{');
    // TODO : class field, static, subroutine compile
    while (tokenizer.hasMoreTokens()) {
        const auto& nextToken = tokenizer.peekToken();

        if (nextToken.type == Token::TokenType::KEYWORD) {
            const auto& kw = std::get<Token::KeywordType>(nextToken.value);

            if (kw == Token::KeywordType::STATIC || kw == Token::KeywordType::FIELD) {
                compileClassVarDec();
            }
            else {
                break;
            }
        }
        else {
            break;
        }
    }

    while (tokenizer.hasMoreTokens()) {
        const auto& nextToken = tokenizer.peekToken();

        if (nextToken.type == Token::TokenType::KEYWORD) {
            const auto& kw = std::get<Token::KeywordType>(nextToken.value);

            if (kw == Token::KeywordType::CONSTRUCTOR || kw == Token::KeywordType::METHOD
                ||kw==Token::KeywordType::FUNCTION) {
                compileSubroutine();
            }
            else {
                break;
            }
        }
        else {
            break;
        }
    }

    process('}');
    ofs << "</class>\n";
}
