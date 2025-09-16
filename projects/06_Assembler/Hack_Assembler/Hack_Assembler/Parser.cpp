//Parser.cpp
//------------------------------------------------------------

#include "Parser.h"
#include <stdexcept> 

//------------------------------------------------------------
//constructor
//------------------------------------------------------------

Parser::Parser(const std::string& filename) {
    ifs.exceptions(std::ios::badbit); // 복구 불가능한 오류에 대해서만 예외 설정
    ifs.open(filename);
    if (!ifs.is_open()) {
        throw std::runtime_error("파일을 열 수 없습니다: " + filename);
    }
}

//--------------------------------------------------------------
//명령어 공백 및 주석 제거 파싱
//--------------------------------------------------------------

bool Parser::advance() {
    while (std::getline(ifs, currentCmd)) {
        auto comment_pos = currentCmd.find("//");
        if (comment_pos != std::string::npos) {
            currentCmd = currentCmd.substr(0, comment_pos);
        }
        auto first = currentCmd.find_first_not_of(" \t\n\r\f\v");
        if (first == std::string::npos) {
            continue;
        }
        auto last = currentCmd.find_last_not_of(" \t\n\r\f\v");
        currentCmd = currentCmd.substr(first, last - first + 1);

        if (!currentCmd.empty()) {
            return true;
        }
    }
    return false;
}

//----------------------------------------------------------------
// 명령어 종류 및 명령어 별 구성 요소 파싱 로직
//----------------------------------------------------------------

instructionType Parser::getInstructionType() const {
    if (currentCmd.empty()) throw std::runtime_error("명령어가 비어있습니다.");
    if (currentCmd[0] == '@') return instructionType::A_INSTRUCTION;
    if (currentCmd[0] == '(') return instructionType::L_INSTRUCTION;
    return instructionType::C_INSTRUCTION;
}

//----------------------------------------------------------------
// A && L instruction has @xxx or (xxx)

std::string Parser::symbol() const {
    instructionType type = getInstructionType();
    if (type == instructionType::A_INSTRUCTION) {
        return currentCmd.substr(1);
    }
    if (type == instructionType::L_INSTRUCTION) {
        return currentCmd.substr(1, currentCmd.length() - 2);
    }
    return ""; // C-명령어일 경우 빈 문자열 반환
}

//----------------------------------------------------------------
// C instruction has (Dest = ) comp (;JOMP) () means optional

std::string Parser::dest() const {
    // C-명령어만 처리
    if (getInstructionType() != instructionType::C_INSTRUCTION) {
        return "";
    }

    auto assign_pos = currentCmd.find('=');
    if (assign_pos != std::string::npos) {
        return currentCmd.substr(0, assign_pos);
    }

    return ""; // '='가 없으면 빈 문자열 반환
}

//----------------------------------------------------------------

std::string Parser::comp() const {
    // ... 기존 코드와 동일
    if (getInstructionType() == instructionType::C_INSTRUCTION) {
        auto assign_pos = currentCmd.find('=');
        auto semicolon_pos = currentCmd.find(';');

        size_t start = (assign_pos == std::string::npos) ? 0 : assign_pos + 1;

        if (semicolon_pos != std::string::npos) {
            return currentCmd.substr(start, semicolon_pos - start);
        }
        return currentCmd.substr(start);
    }
    return "";
}

//----------------------------------------------------------------

std::string Parser::jump() const {
    // C-명령어만 처리
    if (getInstructionType() != instructionType::C_INSTRUCTION) {
        return "";
    }

    auto semicolon_pos = currentCmd.find(';');
    if (semicolon_pos != std::string::npos) {
        return currentCmd.substr(semicolon_pos + 1);
    }

    return ""; // ';'가 없으면 빈 문자열 반환
}