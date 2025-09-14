#include "Parser.h"
#include<exception>

Parser::Parser(std::string& filename)
{
    ifs.exceptions(std::ios::failbit | std::ios::badbit);

    ifs.open(filename);
}

Parser::Parser(const char* filename)
{
    ifs.exceptions(std::ios::badbit);

    ifs.open(filename);
}


bool Parser::advance()
{
    // 파일 끝에 도달할 때까지 루프를 돈다.
    while (std::getline(ifs, currentCmd)) {
        // 1. 주석 제거
        auto comment_pos = currentCmd.find("//");
        if (comment_pos != std::string::npos) {
            currentCmd = currentCmd.substr(0, comment_pos);
        }

        // 2. 앞뒤 공백 제거 (Trim)
        auto first = currentCmd.find_first_not_of(" \t\n\v\r\f");
        if (first == std::string::npos) { // 줄 전체가 공백이나 주석뿐인 경우
            continue; // 루프의 처음으로 돌아가 다음 줄을 읽는다.
        }
        auto last = currentCmd.find_last_not_of(" \t\n\v\r\f");
        currentCmd = currentCmd.substr(first, last - first + 1);

        // 3. 순수한 명령어를 찾았으므로 true를 반환하고 함수 종료
        // currentCmd에 최종 결과가 저장되어 있다.
        return true;
    }

    // while 루프가 끝났다는 것은 파일의 끝에 도달했다는 의미
    return false;
}

instructionType Parser::getInstructionType()
{
    if (currentCmd[0] == '@')return instructionType::A_INSTRUCTION;
    if (currentCmd[0] == '(')return instructionType::L_INSTRUCTION;
    return instructionType::C_INSTRUCTION;
}

std::string Parser::symbol()
{
    instructionType type = getInstructionType();

    if (type == instructionType::A_INSTRUCTION) {
        // '@' 바로 다음 문자부터 끝까지 잘라낸다.
        return currentCmd.substr(1);
    }
    else if (type == instructionType::L_INSTRUCTION) {
        // 첫 번째 '('와 마지막 ')' 사이를 잘라낸다.
        return currentCmd.substr(1, currentCmd.length() - 2);
    }
    else { // C_INSTRUCTION
        return "";
    }
}

std::string Parser::dest()
{
    instructionType type = getInstructionType();
    if (type == instructionType::C_INSTRUCTION) {
        auto assign_pos = currentCmd.find("=");
        
        if (assign_pos == std::string::npos)
            throw std::exception("invalid C instruction format!");

        return currentCmd.substr(0,assign_pos);
    }
    else {
        return "";
    }
}

std::string Parser::comp()
{
    return std::string();
}

std::string Parser::jump()
{
    return std::string();
}
