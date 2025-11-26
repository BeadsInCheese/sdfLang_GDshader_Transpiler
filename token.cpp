#include "token.h"

token::token(token_type t, std::string val) :typ(t), val(val) {


}
std::string token::token_to_string(token_type t) {
    switch (t)
    {
    case token_type::SCALAR:
        return std::string("SCALAR");
    case token_type::OPENPAREN:
        return std::string("OPENPAREN");
    case token_type::CLOSINGPAREN:
        return std::string("CLOSINGPAREN");
    case token_type::NUMBER:
        return std::string("NUMBER");
    case token_type::VEC2:
        return std::string("VEC2");
    case token_type::VEC3:
        return std::string("VEC3");
    case token_type::VEC4:
        return std::string("VEC4");
    case token_type::PLUS:
        return std::string("PLUS");
    case token_type::MINUS:
        return std::string("MINUS");
    case token_type::DIV:
        return std::string("DIV");
    case token_type::MUL:
        return std::string("MUL");
    case token_type::SEMICOLON:
        return std::string("SEMICOLON");
    case token_type::POINT:
        return std::string("POINT");
    case token_type::SHAPE:
        return std::string("SHAPE");
    case token_type::ASSIGN:
        return std::string("ASSIGN");
    case token_type::IDENTIFIER:
        return std::string("IDENTIFIER");
    case token_type::DOT:
        return std::string("DOT");
    case token_type::RETURN:
        return std::string("RETURN");
    case token_type::END_OF_FILE:
        return std::string("EOF");
    case token_type::UNION:
        return std::string("UNION");
    case token_type::INTERSECTION:
        return std::string("INTERSECTION");
    default:
        return "Fail";
        break;
    }
}