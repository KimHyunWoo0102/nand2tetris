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

    // 4. ����� ������ ���� ���� ó��
    Code(const Code&) = delete;
    Code& operator=(const Code&) = delete;

    // 5. �ϸ�а� ���̳ʸ� �ڵ带 ������ ��(map)
    std::map<std::string, std::string> destMap;
    std::map<std::string, std::string> compMap;
    std::map<std::string, std::string> jumpMap;

    // ���� �ʱ�ȭ�ϴ� private ���� �Լ���
    void initializeDestMap();
    void initializeCompMap();
    void initializeJumpMap();

public:
	static Code& getInstance();
	std::string dest(const std::string& mnemonic);
	std::string comp(const std::string& mnemonic);
	std::string jump(const std::string& mnemonic);
};
