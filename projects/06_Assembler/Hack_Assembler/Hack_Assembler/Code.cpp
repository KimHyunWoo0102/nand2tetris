//Code.cpp
//---------------------------------------------------------

#include "Code.h"
#include <exception>

//---------------------------------------------------------
//Mapping 변수
//---------------------------------------------------------

const std::map<std::string, std::string> Code::destMap = {
	{"", "000"}, {"M", "001"}, {"D", "010"}, {"DM", "011"},{"MD","011"},
	{"A", "100"}, {"AM", "101"}, {"AD", "110"}, {"ADM", "111"}
};

//---------------------------------------------------------

const std::map<std::string, std::string> Code::compMap = {
	{"0", "0101010"}, {"1", "0111111"}, {"-1", "0111010"},
	{"D", "0001100"}, {"A","0110000"},	{"M","1110000"},
	{"!D", "0001101"},{"!A", "0110001"},{"!M", "1110001"},
	{"-D", "0001111"},{"-A","0110011"}, {"-M","1110011"},
	{"D+1","0011111"},{"A+1","0110111"},{"M+1","1110111"},
	{"D-1","0001110"},{"A-1","0110010"},{"M-1","1110010"},
	{"D+A","0000010"},{"D+M","1000010"},{"D-A","0010011"},
	{"D-M","1010011"},{"A-D","0000111"},{"M-D","1000111"},
	{"D&A","0000000"},{"D&M","1000000"},{"D|A","0010101"},
	{"D|M","1010101"}
};

//---------------------------------------------------------

const std::map<std::string, std::string> Code::jumpMap = {
	{"", "000"}, {"JGT", "001"}, {"JEQ", "010"}, {"JGE", "011"},
	{"JLT", "100"}, {"JNE", "101"}, {"JLE", "110"}, {"JMP", "111"}
};

//----------------------------------------------------------
// static 변수 리턴
//----------------------------------------------------------

Code& Code::getInstance()
{
	// static 변수는 이 함수가 처음 호출될 때 단 한 번만 생성됩니다.
	// 그 이후부터는 이미 생성된 instance를 계속 반환합니다.
	static Code instance;
	return instance;
}

//----------------------------------------------------------
// 2진수 리턴해주는 헬퍼함수들
//----------------------------------------------------------

std::string Code::dest(const std::string& mnemonic)
{
	auto it = destMap.find(mnemonic);

	if (it != destMap.end()) {
		return it->second;
	}
	else
	{
		auto str = mnemonic + "dest() : invalid mnemonic";
		throw std::exception(str.c_str());
	}
}

//----------------------------------------------------------

std::string Code::comp(const std::string& mnemonic)
{
	auto it = compMap.find(mnemonic);

	if (it != compMap.end()) {
		return it->second;
	}
	else
	{
		throw std::exception("comp() : invalid mnemonic");
	}
}

//----------------------------------------------------------

std::string Code::jump(const std::string& mnemonic)
{
	auto it = jumpMap.find(mnemonic);

	if (it != jumpMap.end()) {
		return it->second;
	}
	else
	{
		throw std::exception("jump() : invalid mnemonic");
	}
}
