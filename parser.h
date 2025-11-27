#pragma once
#include<memory.h>
#include <vector>
#include <iostream>
#include "token.h"
#include <unordered_map>
#include <string>

std::string replaceAll(std::string s,
    const std::string& sequence,
    const std::string& toReplace);

template <typename T,typename U>
std::unique_ptr<U> cast_unique(std::unique_ptr<T> ptr){
    if (U* raw = dynamic_cast<U*>(ptr.release())) {
        return std::unique_ptr<U>(raw);
    }
    std::wcout<<L"ERROR Cast failed"<<"\n";
}
class statement {
public:
    virtual std::wstring print(bool isLast, const std::wstring& prefix);
    virtual std::string emit();
};
enum class expressionType { BASE,ACCESS,POINT, NUMBER, IDENTIFIER, VEC2, VEC3, VEC4, SHAPE, BINARY, UNARY };
class expression {

public:
    virtual token_type getExprType();
    virtual std::wstring print(bool isLast, const std::wstring& prefix);
    virtual std::string emit();
    virtual std::unique_ptr<expression> getAsRHS();
    virtual expressionType getType();
    virtual expression* getProperty(std::string key);
    virtual void setProperty(std::string key,expression* value);
};

struct NamedValue {
    token_type type_information;
    std::unique_ptr<expression> value;
};
class Parser {
    std::unordered_map<token_type, int> bindingPowers = { {token_type::PLUS,1 },{token_type::MINUS,1 },{token_type::MUL,3 },{token_type::DIV,3 },{token_type::OPENPAREN,-1 },{token_type::COMMA,-1 },{token_type::UNION,1 },{token_type::INTERSECTION,1 } };
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
        std::unique_ptr<expression> getAsRHS() override;
        token_type getExprType()override;
        expressionType getType()override;
        expression* getProperty(std::string key)override;
        void setProperty(std::string key, expression* value)override;
    };
    class AccessPropertyExpr :public expression {
    public:
        std::unique_ptr<expression> lhs;
        std::string propertyName;
        std::wstring print(bool isLast, const std::wstring& prefix) override;
        std::string emit()override;
        std::unique_ptr<expression> getAsRHS() override;
        token_type getExprType()override;
        expressionType getType()override;
        expression* getProperty(std::string key)override;
        void setProperty(std::string key, expression* value)override;
    };
    class UnaryExpression :public expression {
    public:
        std::unique_ptr<expression> rhs;
        token oper = token(token_type::PLUS, "");
        std::wstring print(bool isLast, const std::wstring& prefix) override;
        std::string emit()override;
        std::unique_ptr<expression> getAsRHS() override;
        token_type getExprType()override;
        expressionType getType()override;
        expression* getProperty(std::string key)override;
        void setProperty(std::string key, expression* value)override;
    };
    class IdentifierExpression :public expression {
    public:
        std::string identifier;
        std::unordered_map<std::string, NamedValue>* variables;
        IdentifierExpression(std::string v, std::unordered_map<std::string, NamedValue>* variables);
        std::wstring print(bool isLast, const std::wstring& prefix) override;
        std::string emit()override;
        std::unique_ptr<expression> getAsRHS() override;
        token_type getExprType()override;
        expressionType getType()override;
        expression* getProperty(std::string key)override;
        void setProperty(std::string key, expression* value)override;
    };
    class NumberExpression :public expression {
    public:
        float number;
        NumberExpression(std::string value);
        NumberExpression(float value);
        std::wstring print(bool isLast,const std::wstring& prefix) override;
        std::string emit()override;
        std::unique_ptr<expression> getAsRHS() override;
        token_type getExprType()override;
        expressionType getType()override;
        expression* getProperty(std::string key)override;
        void setProperty(std::string key, expression* value)override;
    };
    class PointXExpression :public expression {
    public:
        std::wstring print(bool isLast, const std::wstring& prefix) override;
        std::string emit()override;
        std::unique_ptr<expression> getAsRHS() override;
        token_type getExprType()override;
        expressionType getType()override;
        expression* getProperty(std::string key)override;
        void setProperty(std::string key, expression* value)override;
    };
    class PointYExpression :public expression {
    public:
        std::wstring print(bool isLast, const std::wstring& prefix) override;
        std::string emit()override;
        std::unique_ptr<expression> getAsRHS() override;
        token_type getExprType()override;
        expressionType getType()override;
        expression* getProperty(std::string key)override;
        void setProperty(std::string key, expression* value)override;
    };
    class PointZExpression :public expression {
    public:
        std::wstring print(bool isLast, const std::wstring& prefix) override;
        std::string emit()override;
        std::unique_ptr<expression> getAsRHS() override;
        token_type getExprType()override;
        expressionType getType()override;
        expression* getProperty(std::string key)override;
        void setProperty(std::string key, expression* value)override;
    };
    class PointExpression :public expression {
    public:
        std::unique_ptr<PointXExpression> x=std::make_unique<PointXExpression>();
        std::unique_ptr<PointYExpression> y = std::make_unique<PointYExpression>();
        std::unique_ptr<PointZExpression> z = std::make_unique<PointZExpression>();
        std::wstring print(bool isLast, const std::wstring& prefix) override;
        std::string emit()override;
        std::unique_ptr<expression> getAsRHS() override;
        token_type getExprType()override;
        expressionType getType()override;
        expression* getProperty(std::string key)override;
        void setProperty(std::string key, expression* value)override;
    };

    class Vec2Expression :public expression {
    public:
        std::unique_ptr<expression> x;
        std::unique_ptr<expression> y;
        Vec2Expression(expression* value, expression* value2);
        std::wstring print(bool isLast, const std::wstring& prefix) override;
        std::string emit()override;
        std::unique_ptr<expression> getAsRHS() override;
        token_type getExprType()override;
        expressionType getType()override;
        expression* getProperty(std::string key)override;
        void setProperty(std::string key, expression* value)override;
    };
    class Vec3Expression :public expression {
    public:
        std::unique_ptr<expression> x;
        std::unique_ptr<expression> y;
        std::unique_ptr<expression> z;
        Vec3Expression(expression* value, expression* value2, expression* value3);
        std::wstring print(bool isLast, const std::wstring& prefix) override;
        std::string emit()override;
        std::unique_ptr<expression> getAsRHS() override;
        token_type getExprType()override;
        expressionType getType()override;
        expression* getProperty(std::string key)override;
        void setProperty(std::string key, expression* value)override;
    };
    class Vec4Expression :public expression {
    public:
        std::unique_ptr<expression> x;
        std::unique_ptr<expression> y;
        std::unique_ptr<expression> z;
        std::unique_ptr<expression> w;
        Vec4Expression(expression* value, expression* value2, expression* value3, expression* value4);
        std::wstring print(bool isLast, const std::wstring& prefix) override;
        std::string emit()override;
        std::unique_ptr<expression> getAsRHS() override;
        token_type getExprType()override;
        expressionType getType()override;

        expression* getProperty(std::string key)override;
        void setProperty(std::string key, expression* value)override;
    };
    class ShapeExpression :public expression {
    public:
        std::unique_ptr<expression> sdf;
        std::unique_ptr<Vec3Expression> rot;
        std::unique_ptr<Vec3Expression> pos;
        std::unique_ptr<NumberExpression> scale;
        ShapeExpression(expression* SDF);
        std::wstring print(bool isLast, const std::wstring& prefix) override;
        std::string emit()override;
        std::unique_ptr<expression> getAsRHS() override;
        token_type getExprType()override;
        expressionType getType()override;
        expression* getProperty(std::string key)override;
        void setProperty(std::string key, expression* value)override;
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
        std::unique_ptr<IdentifierExpression> lhs;
        std::unique_ptr<expression> rhs;
        std::wstring print(bool isLast, const std::wstring& prefix) override;
        std::string emit()override;
    };
    class assignPropertyStatement :public statement {
    public:
        std::unique_ptr<IdentifierExpression> lhs;
        std::string property;
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
    std::unique_ptr<expression> parseExpression(std::vector<token>& tokens, int& ptr);

    std::unique_ptr<expression> parseNumberExpression(std::vector<token>& tokens, int& ptr);
    std::unique_ptr<IdentifierExpression> parseIdentifierExpression(std::vector<token>& tokens, int& ptr);
    std::unique_ptr < statement> parseExpressionStatement(std::vector<token>& tokens, int& ptr);
    std::unique_ptr < statement> parseStatement(std::vector<token>& tokens, int& ptr);
    std::unique_ptr <statement> parseBlockStatement(std::vector<token>& tokens, int& ptr);
    std::unique_ptr < statement> parseAssignmentStatement(std::vector<token>& tokens, int& ptr);
    std::unique_ptr < statement> parseAssignPropertyStatement(std::vector<token>& tokens, int& ptr);
    std::unique_ptr < expression> parseAccessPropertyExpr(std::vector<token>& tokens, int& ptr, expression* lhs);
    std::unique_ptr < statement> parseDeclarationStatement(std::vector<token>& tokens, int& ptr);
    std::unique_ptr < statement> parseReturnStatement(std::vector<token>& tokens, int& ptr);

    std::unique_ptr<statement> parseProgram(std::vector<token>& tokens);
};
