#pragma once
#include<memory.h>
#include <vector>
#include <iostream>
#include "token.h"
#include <unordered_map>
#include <string>
class statement {
public:
    virtual std::wstring print(bool isLast, const std::wstring& prefix);
    virtual std::string emit();
};
class expression {
    enum oper { PLUS, MINUS, DIV, MUL, DOT, CROSS };
public:
    virtual std::wstring print(bool isLast, const std::wstring& prefix);
    virtual std::string emit();
};

struct NamedValue {
    token_type type_information;
    std::unique_ptr<expression> value;
    float position[3];
    float rotation[3];
    float scale[3];
};
class Parser {
    std::unordered_map<token_type, int> bindingPowers = { {token_type::PLUS,1 },{token_type::MINUS,1 },{token_type::MUL,3 },{token_type::DIV,3 },{token_type::OPENPAREN,-1 },{token_type::UNION,1 },{token_type::INTERSECTION,1 } };
public:
    std::shared_ptr<std::unordered_map<std::string, NamedValue>> variables = std::make_shared<std::unordered_map<std::string, NamedValue>>();
    bool isOperation(token_type t);
    token peek(std::vector<token>& tokens, int ptr);
    token look(std::vector<token>& tokens, int ptr);
    token consume(std::vector<token>& tokens, int& ptr);
    token expect(std::vector<token>& tokens, int& ptr, token_type t);

    class BinaryExpression :public expression {
        public:
        std::unique_ptr<expression> lhs;
        std::unique_ptr<expression> rhs;
        token oper=token(token_type::PLUS,"");
        std::wstring print(bool isLast, const std::wstring& prefix) override;
        std::string emit()override;
    };
    class UnaryExpression :public expression {
    public:
        std::unique_ptr<expression> rhs;
        token oper = token(token_type::PLUS, "");
        std::wstring print(bool isLast, const std::wstring& prefix) override;
        std::string emit()override;
    };
    class IdentifierExpression :public expression {
    public:
        std::string identifier;
        std::unordered_map<std::string, NamedValue>* variables;
        IdentifierExpression(std::string v, std::unordered_map<std::string, NamedValue>* variables);
        std::wstring print(bool isLast, const std::wstring& prefix) override;
        std::string emit()override;
    };
    class NumberExpression :public expression {
    public:
        float number;
        NumberExpression(std::string value);
        std::wstring print(bool isLast,const std::wstring& prefix) override;
        std::string emit()override;
    };
    class ExpressionStatement :public statement {
    public:
        std::unique_ptr<expression> expr;
        std::string emit()override;
    };
    class returnStatement :public statement {
    public:
        std::unique_ptr<expression> returnValue;
        std::wstring print(bool isLast, const std::wstring& prefix) override;
        std::string emit()override;
    };
    class assignmentStatement :public statement {
    public:
        std::unique_ptr<expression> lhs;
        std::unique_ptr<expression> rhs;
        std::wstring print(bool isLast, const std::wstring& prefix) override;
        std::string emit()override;
    };
    class declarationStatement :public statement {
    public:
        std::unordered_map<std::string, NamedValue>* variables;
        token type_information;
        std::unique_ptr<IdentifierExpression> lhs;
        std::unique_ptr<expression> rhs;
        declarationStatement(token type, std::unique_ptr<IdentifierExpression> left, std::unique_ptr<expression> right,std::unordered_map<std::string, NamedValue>* variables);
        std::wstring print(bool isLast, const std::wstring& prefix) override;
        std::string emit()override;
    };
    class blockStatement:public statement {
    public:
        std::vector<std::unique_ptr<statement>> statements;
        std::wstring print(bool isLast, const std::wstring& prefix) override;
        std::string emit()override;
    };
public:
    std::unique_ptr<expression> parseUnaryExpression(std::vector<token>& tokens, int& ptr);
    std::unique_ptr<expression> parseBinaryExpression(std::vector<token>& tokens, int& ptr,std::unique_ptr<expression> lhs);
    std::unique_ptr<expression> parseExpression(std::vector<token>& tokens, int& ptr,int bp);

    std::unique_ptr<expression> parseNumberExpression(std::vector<token>& tokens, int& ptr);
    std::unique_ptr<IdentifierExpression> parseIdentifierExpression(std::vector<token>& tokens, int& ptr);
    std::unique_ptr < statement> parseExpressionStatement(std::vector<token>& tokens, int& ptr);
    std::unique_ptr < statement> parseStatement(std::vector<token>& tokens, int& ptr);
    std::unique_ptr <statement> parseBlockStatement(std::vector<token>& tokens, int& ptr);
    std::unique_ptr < statement> parseAssignmentStatement(std::vector<token>& tokens, int& ptr);
    std::unique_ptr < statement> parseDeclarationStatement(std::vector<token>& tokens, int& ptr);
    std::unique_ptr < statement> parseReturnStatement(std::vector<token>& tokens, int& ptr);

    std::unique_ptr<statement> parseProgram(std::vector<token>& tokens);
};
