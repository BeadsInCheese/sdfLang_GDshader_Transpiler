#include "lexer.h"
bool tokenizer::isDigit(char t) {
    return t >= 48 && t <= 57;
}
bool tokenizer::isValidInName(char t) {
    return (65 <= t && t <= 90) || (97 <= t && t <= 122 || isDigit(t));
}
std::string tokenizer::findLongestWord(int ptr, std::string& src) {
    std::string greedy;
    if (isDigit(src[ptr])) {
        return greedy;
    }
    while (ptr < src.size()) {
        if (isValidInName(src[ptr])) {
            greedy += src[ptr];
            ptr++;
        }
        else {
            break;
        }
    }
    return greedy;

}
token tokenizer::readNum(int& ptr, std::string& src) {
    std::string num = "";
    while (ptr < src.size()) {
        if (isDigit(src[ptr])) {
            num += src[ptr];
            ptr++;
        }
        else {
            break;
        }

    }
    return token(token_type::NUMBER, num);
}

std::vector<token> tokenizer::tokenize(std::string source) {
    int ptr = 0;
    std::vector<token> tokens{};


    while (ptr < source.size()) {
        std::string longestWord = findLongestWord(ptr, source);
        if (source[ptr] == ' ' || source[ptr] == '\n' || source[ptr] == '\r') {
            //tokens.push_back(token(token_type::OPENPAREN, "("));
            ptr++;
            continue;
        }
        else if (source[ptr] == '(') {
            tokens.push_back(token(token_type::OPENPAREN, "("));
            ptr++;
            continue;
        }
        else if (source[ptr] == ')') {
            tokens.push_back(token(token_type::CLOSINGPAREN, ")"));
            ptr++;
            continue;
        }
        else if (source[ptr] == '+') {
            tokens.push_back(token(token_type::PLUS, "+"));
            ptr++;
            continue;
        }
        else if (source[ptr] == '=') {
            tokens.push_back(token(token_type::ASSIGN, "="));
            ptr++;
            continue;
        }
        else if (source[ptr] == '-') {
            tokens.push_back(token(token_type::MINUS, "-"));
            ptr++;
            continue;
        }
        else if (source[ptr] == '/') {
            tokens.push_back(token(token_type::DIV, "/"));
            ptr++;
            continue;
        }
        else if (source[ptr] == ';') {
            tokens.push_back(token(token_type::SEMICOLON, ";"));
            ptr++;
            continue;
        }
        else if (source[ptr] == '*') {
            tokens.push_back(token(token_type::MUL, "*"));
            ptr++;
            continue;
        }
        else if (source[ptr] == ',') {
            tokens.push_back(token(token_type::COMMA, ","));
            ptr++;
            continue;
        }
        else if (source[ptr] == '.') {
            tokens.push_back(token(token_type::DOT, "."));
            ptr++;
            continue;
        }
        else if (isdigit(source[ptr]))
        {
            tokens.push_back(readNum(ptr, source));
            continue;
        }


        else if (longestWord == "vec4") {

            ptr += 4;
            tokens.push_back(token(token_type::VEC4, "VEC4"));
            continue;
        }
        else if (longestWord == "vec2") {
            ptr += 4;
            tokens.push_back(token(token_type::VEC2, "VEC2"));
            continue;
        }
        else if (longestWord == "vec3") {
            ptr += 4;
            tokens.push_back(token(token_type::VEC3, "VEC3"));
            continue;
        }
        else if (longestWord == "point") {
            ptr += 5;
            tokens.push_back(token(token_type::POINT, "POINT"));
            continue;
        }
        else if (longestWord == "scalar") {
            ptr += 6;
            tokens.push_back(token(token_type::SCALAR, "SCALAR"));
            continue;
        }
        else if (longestWord == "shape") {
            ptr += 5;
            tokens.push_back(token(token_type::SHAPE, "SHAPE"));
            continue;
        }
        else if (longestWord == "union") {
            ptr += 5;
            tokens.push_back(token(token_type::UNION, "UNION"));
            continue;
        }
        else if (longestWord == "intersect") {
            ptr += 9;
            tokens.push_back(token(token_type::INTERSECTION, "INTERSECTION"));
            continue;
        }
        else if (longestWord == "return") {
            ptr += 6;
            tokens.push_back(token(token_type::RETURN, "RETURN"));
            continue;
        }
        else if (longestWord != "") {
            ptr += longestWord.size();
            tokens.push_back(token(token_type::IDENTIFIER, longestWord));
            continue;
        }
        else {
            std::cout << "Unknown token: " << source[ptr] << "\n";
            ptr++;
        }

    }
    return tokens;

}
std::ostream& operator<<(std::ostream& os, token t) {
    return os << "Token" << t.token_to_string(t.typ) << " : " << t.val;
}