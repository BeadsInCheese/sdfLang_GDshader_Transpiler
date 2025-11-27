#pragma once
#include <string>

enum class token_type {END_OF_FILE,CROSS,ONION,STRETCH,REVOLVE,EXTRUDE, IDENTIFIER, RETURN, SCALAR, OPENPAREN, CLOSINGPAREN, NUMBER, VEC2, VEC3, VEC4, PLUS, DOT, MINUS, DIV, MUL, SEMICOLON, POINT, SHAPE, ASSIGN, UNION, INTERSECTION ,COMMA};
class token {
public:

    token_type typ;
    std::string val;
    token(token_type t, std::string val);
    std::string token_to_string(token_type t);

};