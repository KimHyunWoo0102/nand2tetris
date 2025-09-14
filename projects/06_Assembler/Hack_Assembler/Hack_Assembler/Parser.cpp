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
    // ���� ���� ������ ������ ������ ����.
    while (std::getline(ifs, currentCmd)) {
        // 1. �ּ� ����
        auto comment_pos = currentCmd.find("//");
        if (comment_pos != std::string::npos) {
            currentCmd = currentCmd.substr(0, comment_pos);
        }

        // 2. �յ� ���� ���� (Trim)
        auto first = currentCmd.find_first_not_of(" \t\n\v\r\f");
        if (first == std::string::npos) { // �� ��ü�� �����̳� �ּ����� ���
            continue; // ������ ó������ ���ư� ���� ���� �д´�.
        }
        auto last = currentCmd.find_last_not_of(" \t\n\v\r\f");
        currentCmd = currentCmd.substr(first, last - first + 1);

        // 3. ������ ��ɾ ã�����Ƿ� true�� ��ȯ�ϰ� �Լ� ����
        // currentCmd�� ���� ����� ����Ǿ� �ִ�.
        return true;
    }

    // while ������ �����ٴ� ���� ������ ���� �����ߴٴ� �ǹ�
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
        // '@' �ٷ� ���� ���ں��� ������ �߶󳽴�.
        return currentCmd.substr(1);
    }
    else if (type == instructionType::L_INSTRUCTION) {
        // ù ��° '('�� ������ ')' ���̸� �߶󳽴�.
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
