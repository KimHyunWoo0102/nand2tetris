#include "Parser.h"
#include<exception>

Parser::Parser(std::string& filename)
{
    ifs.exceptions(std::ios::failbit | std::ios::badbit);

    ifs.open(filename);
}

Parser::Parser(const char* filename)
{
    ifs.exceptions(std::ios::failbit | std::ios::badbit);

    ifs.open(filename);
}

bool Parser::hasMoreLines() 
{
    //���� �о���µ� �����ߴٸ� ���� ������
    return bool(std::getline(ifs,currentCmd));
}

void Parser::advance(std::string& line)
{
    //check has comment
    auto comment_pos = line.find("//");
    if (comment_pos != std::string::npos) {
        line = line.substr(0, comment_pos);
    }

    auto first = line.find_first_not_of(" \t\n\v\r\f");

    if (first == std::string::npos) {
        line = "";
        return;
    }
    
    auto last = line.find_last_not_of(" \t\n\v\r\f");
    line = line.substr(first, last - first + 1);
}
