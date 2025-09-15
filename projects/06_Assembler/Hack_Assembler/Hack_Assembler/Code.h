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

    // 4. ����� ������ ���� ���� ó��
    Code(const Code&) = delete;
    Code& operator=(const Code&) = delete;

    // 5. �ϸ�а� ���̳ʸ� �ڵ带 ������ ��(map)
    static const std::map<std::string, std::string> destMap;
    static const std::map<std::string, std::string> compMap;
    static const std::map<std::string, std::string> jumpMap;

public:
	static Code& getInstance();
    std::string dest(const std::string& mnemonic);
    std::string comp(const std::string& mnemonic);
    std::string jump(const std::string& mnemonic);
};
