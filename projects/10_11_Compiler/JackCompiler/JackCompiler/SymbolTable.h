#pragma once

#include<string>
#include<unordered_map>


enum class Kind { STATIC, FIELD, ARG, VAR, NONE };

struct SymbolInfo {
	std::string type;
	Kind kind;
	size_t idx;
};

class SymbolTable {
private:
	std::unordered_map<std::string, SymbolInfo> classScopeTable;
	std::unordered_map<std::string, SymbolInfo> subroutineScopeTable;

	size_t varIdx = 0;
	size_t argIdx = 0;
	size_t staticIdx = 0;
	size_t fieldIdx = 0;

public:
	SymbolTable() = default;

	void resetTable();

	void define(const std::string& name, const std::string& type, Kind kind);
	int varCount(Kind kind) const;
	Kind kindOf(const std::string& name) const;
	const std::string& typeOf(const std::string& name) const; // const& 반환이 효율적
	int indexOf(const std::string& name) const;
};