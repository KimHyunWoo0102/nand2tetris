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
        // [���� ó�� 1] ��ȿ���� ���� Kind�� ���� ��� (���� ���� ����)
        throw std::logic_error("Invalid Kind specified in SymbolTable::define");
    }

    // �̸��� �̹� �ش� ���̺� �ִ��� Ȯ��
    if (targetTable->count(name)) {
        // [���� ó�� 2] �̹� ���ǵ� �ɺ� (���� ����)
        throw std::runtime_error("Syntax Error: Symbol '" + name + "' is already defined in this scope.");
    }

    // �̸��� ������ ��� ����
    (*targetTable)[name] = { type, kind, *targetIdx }; // ���� �ε��� ���
    (*targetIdx)++; // ī���� ����
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
        // NONE �Ǵ� �߸��� Kind�� ��� 0�� ��ȯ�ϰų� ���� ó��
        return 0;
    }
}

//----------------------------------------------------------------

Kind SymbolTable::kindOf(const std::string& name) const {
    // 1. �����ƾ ������ ���� �˻�
    if (subroutineScopeTable.count(name)) {
        return subroutineScopeTable.at(name).kind;
    }
    // 2. Ŭ���� ������ ���� �˻�
    else if (classScopeTable.count(name)) {
        return classScopeTable.at(name).kind;
    }
    // 3. �� �� ������ NONE ��ȯ
    else {
        return Kind::NONE;
    }
}

//----------------------------------------------------------------

// �̸�(name)���� Ÿ��(type) ã��
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

// �̸�(name)���� �ε���(index) ã��
int SymbolTable::indexOf(const std::string& name) const {
    if (subroutineScopeTable.count(name)) {
        return subroutineScopeTable.at(name).idx;
    }
    else if (classScopeTable.count(name)) {
        return classScopeTable.at(name).idx;
    }
    else {
        // ã�� ������ ��� -1 ��ȯ (���� ǥ��)
        return -1;
    }
}

//----------------------------------------------------------------
