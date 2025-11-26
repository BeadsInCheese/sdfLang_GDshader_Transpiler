#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include "token.h"
std::ostream& operator<<(std::ostream& os, token t);
class tokenizer {
    bool isDigit(char t);
    bool isValidInName(char t);
    std::string findLongestWord(int ptr, std::string& src);
    token readNum(int& ptr, std::string& src);
public:
    std::vector<token> tokenize(std::string source);

};
