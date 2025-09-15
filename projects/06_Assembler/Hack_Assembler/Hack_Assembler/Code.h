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


class Code {
    Code() = default;
    ~Code() = default;

    // 4. 복사와 대입을 막기 위한 처리
    Code(const Code&) = delete;
    Code& operator=(const Code&) = delete;

    // 5. 니모닉과 바이너리 코드를 저장할 맵(map)
    static const std::map<std::string, std::string> destMap;
    static const std::map<std::string, std::string> compMap;
    static const std::map<std::string, std::string> jumpMap;

public:
	static Code& getInstance();
    std::string dest(const std::string& mnemonic);
    std::string comp(const std::string& mnemonic);
    std::string jump(const std::string& mnemonic);
};
