#include "SymbolTable.h"
#include <stdexcept>

//----------------------------------------------------------------

void SymbolTable::resetTable()
{
	this->subroutineScopeTable.clear();
	this->varIdx = 0;
	this->argIdx = 0;
}

//----------------------------------------------------------------

void SymbolTable::define(const std::string& name, const std::string& type, Kind kind)
{
    std::unordered_map<std::string, SymbolInfo>* targetTable = nullptr;
    // Use int instead of size_t for simplicity, as discussed
    size_t* targetIdx = nullptr;

    if (kind == Kind::ARG) {
        targetIdx = &(this->argIdx);
        targetTable = &(this->subroutineScopeTable);
    }
    else if (kind == Kind::VAR) {
        targetIdx = &(this->varIdx);
        targetTable = &(this->subroutineScopeTable);
    }
    else if (kind == Kind::STATIC) {
        targetIdx = &(this->staticIdx);
        targetTable = &(this->classScopeTable);
    }
    else if (kind == Kind::FIELD) {
        targetIdx = &(this->fieldIdx);
        targetTable = &(this->classScopeTable);
    }
    else {
        // [예외 처리 1] 유효하지 않은 Kind가 들어온 경우 (내부 로직 오류)
        throw std::logic_error("Invalid Kind specified in SymbolTable::define");
    }

    // 이름이 이미 해당 테이블에 있는지 확인
    if (targetTable->count(name)) {
        // [예외 처리 2] 이미 정의된 심볼 (문법 오류)
        throw std::runtime_error("Syntax Error: Symbol '" + name + "' is already defined in this scope.");
    }

    // 이름이 없으면 등록 진행
    (*targetTable)[name] = { type, kind, *targetIdx }; // 현재 인덱스 사용
    (*targetIdx)++; // 카운터 증가
}

//----------------------------------------------------------------

int SymbolTable::varCount(Kind kind) const {
    switch (kind) {
    case Kind::STATIC:
        return staticIdx;
    case Kind::FIELD:
        return fieldIdx;
    case Kind::ARG:
        return argIdx;
    case Kind::VAR:
        return varIdx;
    default:
        // NONE 또는 잘못된 Kind의 경우 0을 반환하거나 오류 처리
        return 0;
    }
}

//----------------------------------------------------------------

Kind SymbolTable::kindOf(const std::string& name) const {
    // 1. 서브루틴 스코프 먼저 검색
    if (subroutineScopeTable.count(name)) {
        return subroutineScopeTable.at(name).kind;
    }
    // 2. 클래스 스코프 다음 검색
    else if (classScopeTable.count(name)) {
        return classScopeTable.at(name).kind;
    }
    // 3. 둘 다 없으면 NONE 반환
    else {
        return Kind::NONE;
    }
}

//----------------------------------------------------------------

// 이름(name)으로 타입(type) 찾기
const std::string& SymbolTable::typeOf(const std::string& name) const {
    static const std::string empty_string = "";

    if (subroutineScopeTable.count(name)) {
        return subroutineScopeTable.at(name).type;
    }
    else if (classScopeTable.count(name)) {
        return classScopeTable.at(name).type;
    }
    else {
        return empty_string;
    }
}

//----------------------------------------------------------------

// 이름(name)으로 인덱스(index) 찾기
int SymbolTable::indexOf(const std::string& name) const {
    if (subroutineScopeTable.count(name)) {
        return subroutineScopeTable.at(name).idx;
    }
    else if (classScopeTable.count(name)) {
        return classScopeTable.at(name).idx;
    }
    else {
        // 찾지 못했을 경우 -1 반환 (오류 표시)
        return -1;
    }
}

//----------------------------------------------------------------
