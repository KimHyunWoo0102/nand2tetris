//Code.h
//----------------------------------------------
#pragma once

//----------------------------------------------
//includes
//----------------------------------------------

#include<string>
#include<iostream>
#include<map>

//----------------------------------------------

using binary = std::string;
using instruction = std::string;

class Code {
    Code();
    ~Code() = default;

    // 4. 복사와 대입을 막기 위한 처리
    Code(const Code&) = delete;
    Code& operator=(const Code&) = delete;

    // 5. 니모닉과 바이너리 코드를 저장할 맵(map)
    std::map<std::string, std::string> destMap;
    std::map<std::string, std::string> compMap;
    std::map<std::string, std::string> jumpMap;

    // 맵을 초기화하는 private 헬퍼 함수들
    void initializeDestMap();
    void initializeCompMap();
    void initializeJumpMap();

public:
	static Code& getInstance();
	std::string dest(const std::string& mnemonic);
	std::string comp(const std::string& mnemonic);
	std::string jump(const std::string& mnemonic);
};
