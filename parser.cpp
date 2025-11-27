#include "parser.h"
bool Parser::isOperation(token_type t)
{
    return t == token_type::PLUS || t == token_type::MINUS || t == token_type::MUL || t == token_type::DIV || t == token_type::OPENPAREN || t == token_type::UNION || t == token_type::INTERSECTION;
}
token Parser::peek(std::vector<token>& tokens, int ptr) {
    if (ptr + 1 >= tokens.size()) {
        return token(token_type::END_OF_FILE,"");
    }
    return tokens[ptr + 1];
}
token Parser::look(std::vector<token>& tokens, int ptr) {
    if (ptr >= tokens.size()) {
        return token(token_type::END_OF_FILE, "");
    }
    return tokens[ptr];
}
token Parser::consume(std::vector<token>& tokens, int& ptr) {
    if (ptr >= tokens.size()) {
        return token(token_type::END_OF_FILE,"");
    }
    token tok = tokens[ptr];
    ptr++;
    return tok;
}
token Parser::expect(std::vector<token>& tokens, int& ptr, token_type t) {
    if (ptr >= tokens.size()) {
        return token(token_type::END_OF_FILE, "");
    }
    token tok = tokens[ptr];
    if (tok.typ != t) {
        std::string s1 = tok.token_to_string(tok.typ);
        std::string s2 = tok.token_to_string(t);
        std::wcout << L"\nPARSER ERROR INVALID TYPE " << std::wstring(s1.begin(),s1.end()) << L" EXPECTED " << std::wstring(s2.begin(), s2.end()) << L"\n";
    }
    ptr++;
    return tok;
}
Parser::IdentifierExpression::IdentifierExpression(std::string v, std::unordered_map<std::string, NamedValue>* variables) {
    identifier = v;
    this->variables = variables;
}
std::wstring Parser::IdentifierExpression::print(bool isLast, const std::wstring& prefix)
{
    return prefix+ L"└── "+std::wstring(identifier.begin(), identifier.end()) + L"\n";
}
std::string Parser::IdentifierExpression::emit()
{
    return (*variables)[identifier].value->emit();
}
std::unique_ptr<expression> Parser::IdentifierExpression::getAsRHS()
{
    return (*variables)[identifier].value->getAsRHS();
}
token_type Parser::IdentifierExpression::getExprType()
{
    return (*variables)[identifier].type_information;
}
expressionType Parser::IdentifierExpression::getType()
{
    return expressionType::IDENTIFIER;
}
expression* Parser::IdentifierExpression::getProperty(std::string key)
{
    return (*variables)[identifier].value->getProperty(key);
}
void Parser::IdentifierExpression::setProperty(std::string key, expression* value)
{
    return (*variables)[identifier].value->setProperty(key, value);
}
std::wstring Parser::NumberExpression::print(bool isLast, const std::wstring& prefix)
{
    return prefix+ L"└── "+std::to_wstring(number) + L"\n";
}
std::string Parser::NumberExpression::emit()
{
    return "{"+std::to_string(number)+"}";
}
std::unique_ptr<expression> Parser::NumberExpression::getAsRHS()
{

    return std::make_unique<NumberExpression>(number);
}
token_type Parser::NumberExpression::getExprType()
{
    return token_type::SCALAR;
}
expressionType Parser::NumberExpression::getType()
{
    return expressionType::NUMBER;
}
expression* Parser::NumberExpression::getProperty(std::string key)
{
    return nullptr;
}
void Parser::NumberExpression::setProperty(std::string key, expression* value)
{
}
std::unique_ptr<expression> expression::getAsRHS(){
    return std::make_unique<expression>();
}
expressionType expression::getType()
{
    return expressionType::BASE;
}
expression* expression::getProperty(std::string key)
{
    return nullptr;
}
void expression::setProperty(std::string key, expression* value)
{
}
token_type expression::getExprType()
{
    return token_type::END_OF_FILE;
}
std::wstring expression::print(bool isLast, const std::wstring& prefix)
{
    return prefix+ L"└── default expression. Some error has happened during parsing\n";
}


Parser::NumberExpression::NumberExpression(std::string value) {
    number = std::stof(value);
}
Parser::NumberExpression::NumberExpression(float value):number(value)
{
    
}
std::wstring Parser::BinaryExpression::print(bool isLast, const std::wstring& prefix)
{
    std::wstring out = prefix + (isLast ? L"└── " : L"├── ")
        + std::wstring(oper.val.begin(), oper.val.end()) + L"\n";

    std::wstring childPrefix = prefix + (isLast ? L"    " : L"│   ");

    out += lhs->print(false, childPrefix);

    out += rhs->print(true, childPrefix);

    return out;
}

std::string Parser::BinaryExpression::emit()
{
    std::string code = std::string("");
    switch (oper.typ) {
    case token_type::MINUS:
        if (lhs->getExprType() == token_type::SCALAR && rhs->getExprType() == token_type::SCALAR) {
            
        }
        code += lhs->emit() + "-" + rhs->emit();
        break;
    case token_type::PLUS:
        code += lhs->emit() + "+" + rhs->emit();
        break;
    case token_type::MUL:
        code += lhs->emit() + "*" + rhs->emit();
        break;
    case token_type::DIV:
        code += lhs->emit() + "/" + rhs->emit();
        break;
    case token_type::UNION:
        code += "smoothUnion(" + lhs->emit() + "," + rhs->emit()+")";
        break;
    case token_type::INTERSECTION:
        code += "smoothIntersect(" + lhs->emit() + "," + rhs->emit() + ")";
        break;
    }

    return code;
}

std::unique_ptr<expression> Parser::BinaryExpression::getAsRHS()
{

    auto trhs = rhs->getAsRHS();
    auto tlhs = lhs->getAsRHS();
    if (trhs->getType() == expressionType::NUMBER && trhs->getType() == expressionType::NUMBER) {
        if (oper.val == "+") {
            return std::make_unique<NumberExpression>(dynamic_cast<NumberExpression*>(trhs.get())->number + dynamic_cast<NumberExpression*>(tlhs.get())->number);
        }
        if (oper.val == "-") {
            return std::make_unique<NumberExpression>(dynamic_cast<NumberExpression*>(trhs.get())->number - dynamic_cast<NumberExpression*>(tlhs.get())->number);
        }
        if (oper.val == "*") {
            return std::make_unique<NumberExpression>(dynamic_cast<NumberExpression*>(trhs.get())->number * dynamic_cast<NumberExpression*>(tlhs.get())->number);
        }
        if (oper.val == "/") {
            return std::make_unique<NumberExpression>(dynamic_cast<NumberExpression*>(trhs.get())->number / dynamic_cast<NumberExpression*>(tlhs.get())->number);
        }
    }
    std::unique_ptr<BinaryExpression> b = std::make_unique<BinaryExpression>();
    b->rhs = std::move(trhs);
    b->lhs = std::move(tlhs);
    b->oper = oper;
    return std::move(b);
}

token_type Parser::BinaryExpression::getExprType()
{
    token_type l = lhs->getExprType();
    token_type r = rhs->getExprType();
    if (l == token_type::SCALAR && l == token_type::SCALAR) {
        return token_type::SCALAR;
    }
    if (l == token_type::VEC2 && l == token_type::VEC2) {
        return token_type::VEC2;
    }
    if (l == token_type::VEC3 && l == token_type::VEC3) {
        return token_type::VEC3;
    }
    if (l == token_type::VEC4 && l == token_type::VEC4) {
        return token_type::VEC4;
    }
    if (l == token_type::SHAPE && l == token_type::SHAPE) {
        return token_type::SHAPE;
    }
    return token_type();
}

expressionType Parser::BinaryExpression::getType()
{
    return expressionType::BINARY;
}

expression* Parser::BinaryExpression::getProperty(std::string key)
{
    return nullptr;
}

void Parser::BinaryExpression::setProperty(std::string key, expression* value)
{
}

std::wstring Parser::UnaryExpression::print(bool isLast, const std::wstring& prefix)
{
    std::wstring out = prefix + (isLast ? L"└── " : L"├── ")
        + std::wstring(oper.val.begin(), oper.val.end()) + L"\n";

    std::wstring childPrefix = prefix + (isLast ? L"    " : L"│   ");
    out += rhs->print(true, childPrefix);

    return out;
}

std::string Parser::UnaryExpression::emit()
{
    std::string code = std::string("");
    switch (oper.typ) {
    case token_type::MINUS:
        code += "-" + rhs->emit();
        break;
    case token_type::PLUS:
        code +=  "+" + rhs->emit();
        break;
    default:
        break;
    }
    return code;
}

std::unique_ptr<expression> Parser::UnaryExpression::getAsRHS()
{
    auto trhs= rhs->getAsRHS();
    if (trhs->getType() == expressionType::NUMBER) {
        if (oper.val == "+") {
            return std::make_unique<NumberExpression>(dynamic_cast<NumberExpression*>(trhs.get())->number);
        }
        if (oper.val == "-") {
            return std::make_unique<NumberExpression>(-dynamic_cast<NumberExpression*>(trhs.get())->number);
        }
    }
    std::unique_ptr<UnaryExpression> unary = std::make_unique<UnaryExpression>();
    unary->oper = oper;
    unary->rhs = rhs->getAsRHS();
    return std::move(unary);
}

token_type Parser::UnaryExpression::getExprType()
{
    return rhs->getExprType();
}

expressionType Parser::UnaryExpression::getType()
{
    return expressionType::UNARY;
}

expression* Parser::UnaryExpression::getProperty(std::string key)
{
    return nullptr;
}

void Parser::UnaryExpression::setProperty(std::string key, expression* value)
{
}





std::unique_ptr<expression> Parser::parseNumberExpression(std::vector<token>& tokens, int& ptr){
    std::string num=expect(tokens, ptr, token_type::NUMBER).val;

    if (peek(tokens, ptr).typ == token_type::DOT) {
        expect(tokens, ptr, token_type::DOT);
        num += ".";
        num += expect(tokens, ptr, token_type::NUMBER).val;
    }
    return std::make_unique<NumberExpression>(num);
}
std::unique_ptr<expression> Parser::parseUnaryExpression(std::vector<token>& tokens, int& ptr){
    
   if (look(tokens, ptr).typ == token_type::OPENPAREN) {
        consume(tokens, ptr);
        std::unique_ptr<expression> expr = parseExpression(tokens, ptr, -1);
        expect(tokens, ptr, token_type::CLOSINGPAREN);
        return std::move(expr);

        }
    else if (isOperation(look(tokens,ptr).typ)) {
        token tok = expect(tokens, ptr, look(tokens,ptr).typ);
        std::unique_ptr<UnaryExpression> unary = std::make_unique<UnaryExpression>();
        unary->oper = tok;
        unary->rhs = parseExpression(tokens, ptr,5);
        return std::move(unary);
    }

    else if (look(tokens, ptr).typ == token_type::NUMBER) {
        return parseNumberExpression(tokens,ptr);

    }
    else if (look(tokens, ptr).typ == token_type::VEC2) {
       consume(tokens, ptr);
       expect(tokens, ptr, token_type::OPENPAREN);
       std::unique_ptr<expression> param1 = parseExpression(tokens, ptr);
       expect(tokens, ptr, token_type::COMMA);
       std::unique_ptr<expression> param2 = parseExpression(tokens, ptr);
       expect(tokens, ptr, token_type::CLOSINGPAREN);
       return std::make_unique<Vec2Expression>(param1.get(),param2.get());

   }
    else if (look(tokens, ptr).typ == token_type::VEC3) {
       consume(tokens, ptr);
       expect(tokens, ptr, token_type::OPENPAREN);
       std::unique_ptr<expression> param1 = parseExpression(tokens, ptr);
       expect(tokens, ptr, token_type::COMMA);
       std::unique_ptr<expression> param2 = parseExpression(tokens, ptr);
       expect(tokens, ptr, token_type::COMMA);
       std::unique_ptr<expression> param3 = parseExpression(tokens, ptr);
       expect(tokens, ptr, token_type::CLOSINGPAREN);
       return std::make_unique<Vec3Expression>(param1.get(), param2.get(),param3.get());

   }
    else if (look(tokens, ptr).typ == token_type::VEC4) {
       consume(tokens, ptr);
       expect(tokens, ptr, token_type::OPENPAREN);
       std::unique_ptr<expression> param1 = parseExpression(tokens, ptr);
       expect(tokens, ptr, token_type::COMMA);
       std::unique_ptr<expression> param2 = parseExpression(tokens, ptr);
       expect(tokens, ptr, token_type::COMMA);
       std::unique_ptr<expression> param3 = parseExpression(tokens, ptr);
       expect(tokens, ptr, token_type::COMMA);
       std::unique_ptr<expression> param4 = parseExpression(tokens, ptr);
       expect(tokens, ptr, token_type::CLOSINGPAREN);
       return std::make_unique<Vec4Expression>(param1.get(), param2.get(), param3.get(),param4.get());

   }
    else if (look(tokens, ptr).typ == token_type::SHAPE) {
       consume(tokens, ptr);
       expect(tokens, ptr, token_type::OPENPAREN);
       std::unique_ptr<expression> param1 = parseExpression(tokens, ptr);
       expect(tokens, ptr, token_type::CLOSINGPAREN);
       return std::make_unique<ShapeExpression>(param1.get());
   }
    else if (look(tokens, ptr).typ == token_type::IDENTIFIER) {
       if (peek(tokens, ptr).typ == token_type::DOT) {
           return parseAccessPropertyExpr(tokens,ptr);
        }
        return parseIdentifierExpression(tokens,ptr);

    }
    else if (look(tokens, ptr).typ == token_type::POINT) {
        consume(tokens,ptr);
        return std::make_unique<PointExpression>();

    }
    std::wcout << L"WTF invalid expression" << "\n";
    return std::make_unique<expression>();
}

std::unique_ptr<expression> Parser::parseBinaryExpression(std::vector<token>& tokens, int& ptr,std::unique_ptr<expression> lhs){
    std::unique_ptr<BinaryExpression> b = std::make_unique<BinaryExpression>();

    b->lhs = std::move(lhs);
    b->oper = consume(tokens,ptr);
    b->rhs = parseExpression(tokens, ptr,-1);
    return std::move(b);
}
std::unique_ptr<expression> Parser::parseExpression(std::vector<token>& tokens, int& ptr,int bp) {
        std::unique_ptr<expression> first = parseUnaryExpression(tokens, ptr);
        
        std::unique_ptr<expression> led=std::move(first);
        token oper = token(token_type::END_OF_FILE,"EOF");
        while (isOperation(look(tokens, ptr).typ) &&bindingPowers[look(tokens,ptr).typ] > bp) {
            oper = consume(tokens,ptr);
            //std::cout << "opr: " << oper.val << "\n";
            std::unique_ptr<BinaryExpression> b = std::make_unique<BinaryExpression>();
            b->lhs = std::move(led);
            b->oper = oper;
            b->rhs = parseExpression(tokens, ptr, bindingPowers[oper.typ]);
            led = std::move(b);
        }
        return led;
        
    std::cout << "ERROR could not parse expression.\n"; 
    return std::make_unique<expression>();
}
std::unique_ptr<expression> Parser::parseExpression(std::vector<token>& tokens, int& ptr)
{
    return parseExpression(tokens, ptr, -1);
}
std::unique_ptr<Parser::IdentifierExpression> Parser::parseIdentifierExpression(std::vector<token>& tokens, int& ptr) {
    return std::make_unique<IdentifierExpression>(consume(tokens, ptr).val,variables.get());

}

std::unique_ptr<statement> Parser::parseExpressionStatement(std::vector<token>& tokens, int& ptr)
{
    std::wcout << L"expression statement.\n";
    std::unique_ptr<ExpressionStatement> expr = std::make_unique<ExpressionStatement>();
    expr->expr = parseExpression(tokens, ptr,-1);
    expect(tokens, ptr,token_type::SEMICOLON);
    return std::move(expr);
}

std::unique_ptr<statement> Parser::parseStatement(std::vector<token>& tokens, int& ptr)
{
    std::string s = look(tokens, ptr).token_to_string(look(tokens, ptr).typ);
    std::wcout << L"parse statement.\n"+ std::wstring(s.begin(),s.end());
    if (look(tokens, ptr).typ == token_type::VEC2 || look(tokens, ptr).typ == token_type::VEC3 || look(tokens, ptr).typ == token_type::VEC4 || look(tokens, ptr).typ == token_type::SCALAR || look(tokens, ptr).typ == token_type::SHAPE) {
        return parseDeclarationStatement(tokens, ptr);
    }
    if (look(tokens, ptr).typ == token_type::IDENTIFIER) {
        if (peek(tokens, ptr).typ == token_type::DOT) {
            return parseAssignPropertyStatement(tokens,ptr);
        }
        return parseAssignmentStatement(tokens, ptr);
    }
    if (look(tokens, ptr).typ == token_type::RETURN) {
        return parseReturnStatement(tokens, ptr);
    }
    //return parseExpressionStatement(tokens,ptr);
    ptr++;
    return std::make_unique<statement>();
}

std::unique_ptr <statement> Parser::parseBlockStatement(std::vector<token>& tokens, int& ptr) {
    std::wcout << L"block statement.\n";
    std::unique_ptr<blockStatement> b = std::make_unique<blockStatement>();

    while (ptr < tokens.size()) {
        b->statements.push_back(parseStatement(tokens, ptr));
    }
    return std::move(b);
}

std::unique_ptr < statement> Parser::parseAssignmentStatement(std::vector<token>& tokens, int& ptr) {
    std::unique_ptr<assignmentStatement> assignment=std::make_unique<assignmentStatement>();
    
    assignment->lhs = parseIdentifierExpression(tokens, ptr);
    token t = expect(tokens, ptr, token_type::ASSIGN);
    assignment->rhs = parseExpression(tokens, ptr,-1);
    expect(tokens, ptr, token_type::SEMICOLON);
    return std::move(assignment);
}

std::unique_ptr<statement> Parser::parseAssignPropertyStatement(std::vector<token>& tokens, int& ptr)
{
    std::unique_ptr<assignPropertyStatement> assignment = std::make_unique<assignPropertyStatement>();

    assignment->lhs = parseIdentifierExpression(tokens, ptr);
    expect(tokens, ptr, token_type::DOT);
    assignment->property = consume(tokens, ptr).val;
    expect(tokens, ptr, token_type::ASSIGN);
    assignment->rhs = parseExpression(tokens, ptr, -1);
    expect(tokens, ptr, token_type::SEMICOLON);
    return std::move(assignment);
}

std::unique_ptr<expression> Parser::parseAccessPropertyExpr(std::vector<token>& tokens, int& ptr)
{
    std::unique_ptr<AccessPropertyExpr> access = std::make_unique<AccessPropertyExpr>();
    access->lhs = parseIdentifierExpression(tokens, ptr);
    expect(tokens, ptr, token_type::DOT);
    access->propertyName = consume(tokens, ptr).val;
    return std::move(access);
}


std::unique_ptr<statement> Parser::parseDeclarationStatement(std::vector<token>& tokens, int& ptr)
{
    token t= consume(tokens, ptr);
    auto lhs = parseIdentifierExpression(tokens, ptr);
    expect(tokens, ptr, token_type::ASSIGN);
    auto rhs = parseExpression(tokens, ptr, -1);
    std::unique_ptr<declarationStatement> assignment = std::make_unique<declarationStatement>(t,std::move(lhs),std::move(rhs),variables.get());

    expect(tokens, ptr, token_type::SEMICOLON);
    return std::move(assignment);
}

std::unique_ptr < statement> Parser::parseReturnStatement(std::vector<token>& tokens, int& ptr) {
    expect(tokens, ptr, token_type::RETURN);
    std::unique_ptr<expression> value = parseExpression(tokens, ptr,-1);
    expect(tokens, ptr, token_type::SEMICOLON);
    std::unique_ptr<returnStatement> r=std::make_unique<returnStatement>();
    r->returnValue = std::move(value);
    return std::move(r);


}

std::unique_ptr<statement> Parser::parseProgram(std::vector<token>& tokens) {
    int ptr = 0;
    return parseBlockStatement(tokens, ptr);
}
std::wstring statement::print(bool isLast, const std::wstring& prefix) {
    return L"";
}
std::string statement::emit()
{
    return std::string("");
}
std::string expression::emit()
{
    return std::string("ERROR");
}
std::wstring Parser::assignmentStatement::print(bool isLast, const std::wstring& prefix)
{
    std::wstring out = prefix + (isLast ? L"└── " : L"├── ") + L"assign" + L"\n";

    std::wstring childPrefix = prefix + (isLast ? L"    " : L"│   ");
    out += lhs->print(true, childPrefix);
    out += rhs->print(true, childPrefix);

    return out;
}
std::string Parser::assignmentStatement::emit()
{
    (*lhs->variables)[lhs->identifier].value = rhs->getAsRHS();
    return "";
}
std::wstring Parser::blockStatement::print(bool isLast, const std::wstring& prefix)
{
    std::wstring out = prefix + (isLast ? L"└── " : L"├── ") + L"{block}" + L"\n";

    std::wstring childPrefix = prefix + (isLast ? L"    " : L"│   ");
    for (size_t i = 0; i < statements.size(); ++i) {
        bool last = (i + 1 == statements.size());
        out += statements[i]->print(last, childPrefix);
    }

    return out;
}

std::string Parser::blockStatement::emit()
{
    std::string code = std::string("");
    for (  auto& i:statements)
    {
        code += i->emit();
    }
    return code;
}

std::wstring Parser::returnStatement::print(bool isLast, const std::wstring& prefix)
{
    std::wstring out = prefix + (isLast ? L"└── " : L"├── ") + L"return" + L"\n";

    std::wstring childPrefix = prefix + (isLast ? L"    " : L"│   ");
    if (returnValue) {
        out += returnValue->print(true, childPrefix);
    }

    return out;
}

std::string Parser::returnStatement::emit()
{
    return "return " + returnValue->getAsRHS()->emit() + ";";
}

std::string Parser::ExpressionStatement::emit()
{
    return expr->emit()+";";
}

Parser::declarationStatement::declarationStatement(
    token t,
    std::unique_ptr<IdentifierExpression> left,
    std::unique_ptr<expression> right,
    std::unordered_map<std::string, NamedValue>* variables
) : type_information(t), lhs(std::move(left)), rhs(std::move(right)), variables(variables) {
}


std::wstring Parser::declarationStatement::print(bool isLast, const std::wstring& prefix)
{
    std::string t = type_information.token_to_string(type_information.typ);
    std::wstring out = prefix + (isLast ? L"└── " : L"├── ") + L"declare " + std::wstring(t.begin(),t.end()) + L"\n";

    std::wstring childPrefix = prefix + (isLast ? L"    " : L"│   ");
    out += lhs->print(true, childPrefix);
    out += rhs->print(true, childPrefix);
    return out;
}

std::string Parser::declarationStatement::emit()
{
    NamedValue n;
    n.type_information = type_information.typ;
    n.value = rhs->getAsRHS();
    (*variables)[lhs->identifier] = std::move(n);
    return std::string("");
};



Parser::Vec2Expression::Vec2Expression(expression* value, expression* value2)
{
    x = value->getAsRHS();
    y = value2->getAsRHS();
}

std::wstring Parser::Vec2Expression::print(bool isLast, const std::wstring& prefix)
{
    return  prefix + L"└── vec2()\n";
}

std::string Parser::Vec2Expression::emit()
{
    return "vec2(" + x->emit() + "," + y->emit() + ")";
}

std::unique_ptr<expression> Parser::Vec2Expression::getAsRHS()
{
    return std::make_unique<Vec2Expression>(x.get(), y.get());
}

token_type Parser::Vec2Expression::getExprType()
{
    return token_type::VEC2;
}

expressionType Parser::Vec2Expression::getType()
{
    return expressionType::VEC2;
}

expression* Parser::Vec2Expression::getProperty(std::string key)
{
    if (key == "x") {
        return x.get();
    }
    if (key == "y") {
        return y.get();
    }
    return nullptr;
}

void Parser::Vec2Expression::setProperty(std::string key, expression* value)
{
    if (value->getExprType() == token_type::SCALAR) {
        if (key == "x") {
            x = value->getAsRHS();
        }
        if (key == "y") {
            y = value->getAsRHS();
        }
    }
    
}



Parser::Vec3Expression::Vec3Expression(expression* value,expression* value2, expression* value3)
{
    x = value->getAsRHS();
    y = value2->getAsRHS();
    z = value3->getAsRHS();
}

std::wstring Parser::Vec3Expression::print(bool isLast, const std::wstring& prefix)
{
    return  prefix + L"└── vec3()\n";
}

std::string Parser::Vec3Expression::emit()
{
    return "vec3(" + x->emit() + "," + y->emit() + ","+z->emit() + ")";
}

std::unique_ptr<expression> Parser::Vec3Expression::getAsRHS()
{
    return std::make_unique<Vec3Expression>(x.get(), y.get(), z.get());
}

token_type Parser::Vec3Expression::getExprType()
{
    return token_type::VEC3;
}

expressionType Parser::Vec3Expression::getType()
{
    return expressionType::VEC3;
}

expression* Parser::Vec3Expression::getProperty(std::string key)
{
    if (key == "x") {
        return x.get();
    }
    if (key == "y") {
        return y.get();
    }
    if (key == "z") {
        return z.get();
    }

    return nullptr;
}

void Parser::Vec3Expression::setProperty(std::string key, expression* value)
{
    if (value->getExprType() == token_type::SCALAR) {
        if (key == "x") {
            x = value->getAsRHS();
        }
        if (key == "y") {
            y = value->getAsRHS();
        }
        if (key == "z") {
            z = value->getAsRHS();
        }
    }
}


Parser::Vec4Expression::Vec4Expression(expression* value, expression* value2, expression* value3, expression* value4)
{
    
    x = value->getAsRHS();
    y = value2->getAsRHS();
    z = value3->getAsRHS();
    w= value4->getAsRHS();

}

std::wstring Parser::Vec4Expression::print(bool isLast, const std::wstring& prefix)
{
    return prefix + L"└──vec4()\n";
}

std::string Parser::Vec4Expression::emit()
{
    return "vec4(" + x->emit() + "," + y->emit() + "," + z->emit() + w->emit() + ")";
}

std::unique_ptr<expression> Parser::Vec4Expression::getAsRHS()
{
    return std::make_unique<Vec4Expression>(x.get(), y.get(), z.get(), w.get());
}

token_type Parser::Vec4Expression::getExprType()
{
    return token_type::VEC4;
}

expressionType Parser::Vec4Expression::getType()
{
    return expressionType::VEC4;
}

expression* Parser::Vec4Expression::getProperty(std::string key)
{
    if (key == "x") {
        return x.get();
    }
    if (key == "y") {
        return y.get();
    }
    if (key == "z") {
        return z.get();
    }
    if (key == "w") {
        return w.get();
    }
    return nullptr;
}

void Parser::Vec4Expression::setProperty(std::string key, expression* value)
{
    if (value->getExprType() == token_type::SCALAR) {
        if (key == "x") {
            x = value->getAsRHS();
        }
        if (key == "y") {
            y = value->getAsRHS();
        }
        if (key == "z") {
            z = value->getAsRHS();
        }
        if (key == "w") {
            w = value->getAsRHS();
        }
    }
}

Parser::ShapeExpression::ShapeExpression(expression* SDF)
{
    sdf = SDF->getAsRHS();
    rot = std::make_unique<Vec3Expression>(std::make_unique < NumberExpression>(0).get(), std::make_unique < NumberExpression>(0).get(), std::make_unique < NumberExpression>(0).get());
    pos = std::make_unique<Vec3Expression>(std::make_unique < NumberExpression>(0).get(), std::make_unique < NumberExpression>(0).get(), std::make_unique < NumberExpression>(0).get());
    scale = std::make_unique<NumberExpression>(1);

}

std::wstring Parser::ShapeExpression::print(bool isLast, const std::wstring& prefix)
{
    return prefix + L"└── shape \n";
}

std::string Parser::ShapeExpression::emit()
{
    float px;
    float py;
    float pz;
    float rx;
    float ry;
    float rz;
    if (pos->x->getType() == expressionType::NUMBER) {
            px=dynamic_cast<NumberExpression*>(pos->x.get())->number;
    }else{
        std::wcout << L"shape pos must be available at compile time. \n";
    }
    if (pos->y->getType() == expressionType::NUMBER) {
        py=dynamic_cast<NumberExpression*>(pos->y.get())->number;
    }
    else {
        std::wcout << L"shape pos must be available at compile time. \n";
    }
    if (pos->z->getType() == expressionType::NUMBER) {
        pz=dynamic_cast<NumberExpression*>(pos->z.get())->number;
    }
    else {
        std::wcout << L"shape pos must be available at compile time. \n";
    }
    if (rot->x->getType() == expressionType::NUMBER) {
        rx=dynamic_cast<NumberExpression*>(rot->x.get())->number;
    }
    else {
        std::wcout << L"shape pos must be available at compile time. \n";
    }
    if (rot->y->getType() == expressionType::NUMBER) {
        ry=dynamic_cast<NumberExpression*>(rot->y.get())->number;
    }
    else {
        std::wcout << L"shape pos must be available at compile time. \n";
    }
    if (rot->z->getType() == expressionType::NUMBER) {
        rz=dynamic_cast<NumberExpression*>(rot->z.get())->number;
    }
    else {
        std::wcout << L"shape pos must be available at compile time. \n";
    }
    std::string sc = scale->emit();

    

    std::string vec1 = "vec4("+std::to_string(std::cos(  ry  )*std::cos( rz ))+", "
        + std::to_string(sin( rx )*sin( ry )*cos( rz ) + cos( rx  )*sin( rz ))+", "
        + std::to_string(-cos( rx )*sin( ry )*cos(rz ) + sin( rx )*sin( rz  ))+", 0)";

    std::string vec2 = "vec4("+std::to_string(-cos( ry  )*sin( rz  ))+", "+
        std::to_string(-sin( rx  )*sin( ry )*sin( rz  ) + cos( rx )*cos( rz  ))+", "+
        std::to_string(cos( rx  )*sin( ry )*sin( rz  ) + sin( rx )*cos( rz ))+", 0)";

    std::string vec3 = "vec4("+ std::to_string(sin( ry ))+", "
        + std::to_string(-sin( rx  )*cos(  ry  ))+", "
        + std::to_string(cos(rx )*cos(  ry ))+", 0)";

    std::string vec4 = "vec4(" + std::to_string(px) + "," + std::to_string(py) + "," + std::to_string(pz) + ",1)";

    return "(" + replaceAll(sdf->emit(), "POINT", "invert(mat4("+ vec1+vec2+vec3+vec4+ "))*POINT/" + sc) + "*" + sc + ")";
}
     
std::unique_ptr<expression> Parser::ShapeExpression::getAsRHS()
{
    auto shape=std::make_unique<ShapeExpression>(sdf.get());
    shape->rot = cast_unique<expression,Vec3Expression>(rot->getAsRHS());
    shape->pos = cast_unique<expression, Vec3Expression>(pos->getAsRHS());
    shape->scale = cast_unique<expression, NumberExpression>(scale->getAsRHS());
    return std::move(shape);
}

token_type Parser::ShapeExpression::getExprType()
{
    return token_type::SHAPE;
}

expressionType Parser::ShapeExpression::getType()
{
    return expressionType::SHAPE;
}

expression* Parser::ShapeExpression::getProperty(std::string key)
{
    if (key == "pos") {
        return pos.get();
    }
    if (key == "rot") {
        return rot.get();
    }
    if (key == "scale") {
        return scale.get();
    }
    return nullptr;
}

void Parser::ShapeExpression::setProperty(std::string key, expression* value)
{
    if (value->getExprType() == token_type::VEC3) {
        if (key == "pos") {
            pos = std::unique_ptr<Vec3Expression>(dynamic_cast<Vec3Expression*>(value->getAsRHS().release()));
        }
        if (key == "rot") {
            rot = std::unique_ptr<Vec3Expression>(dynamic_cast<Vec3Expression*>(value->getAsRHS().release()));
        }
    }
    
    if (value->getExprType() == token_type::SCALAR) {
        if (key == "scale") {
            scale = std::unique_ptr<NumberExpression>(dynamic_cast<NumberExpression*>(value->getAsRHS().release()));
        }
    }
}

std::string replaceAll(std::string s, const std::string& sequence, const std::string& toReplace)
{
    size_t pos = 0;
    while ((pos = s.find(sequence, pos)) != std::string::npos) {
        s.replace(pos, sequence.size(), toReplace);
        pos += toReplace.size();
    }
    return s;
}

std::wstring Parser::PointExpression::print(bool isLast, const std::wstring& prefix)
{
    return prefix + L"└── point \n";
}

std::string Parser::PointExpression::emit()
{
    return "POINT";
}

std::unique_ptr<expression> Parser::PointExpression::getAsRHS()
{
    return std::make_unique<PointExpression>();
}

token_type Parser::PointExpression::getExprType()
{
    return token_type::POINT;
}

expressionType Parser::PointExpression::getType()
{
    return expressionType::POINT;
}

expression* Parser::PointExpression::getProperty(std::string key)
{
    if (key == "x") {
        return x.get();
    }
    if (key == "y") {
        return y.get();
    }
    if (key == "z") {
        return z.get();
    }
    return nullptr;
}

void Parser::PointExpression::setProperty(std::string key, expression* value)
{
}

std::wstring Parser::assignPropertyStatement::print(bool isLast, const std::wstring& prefix)
{
    return std::wstring();
}

std::string Parser::assignPropertyStatement::emit()
{
    lhs->setProperty(property, rhs.get());
    return std::string();
}

std::wstring Parser::PointXExpression::print(bool isLast, const std::wstring& prefix)
{
    return std::wstring();
}

std::string Parser::PointXExpression::emit()
{
    return "POINT.x";
}

std::unique_ptr<expression> Parser::PointXExpression::getAsRHS()
{
    return std::unique_ptr<PointXExpression>();
}

token_type Parser::PointXExpression::getExprType()
{
    return token_type::SCALAR;
}

expressionType Parser::PointXExpression::getType()
{
    return expressionType::POINT;
}

expression* Parser::PointXExpression::getProperty(std::string key)
{
    return nullptr;
}

void Parser::PointXExpression::setProperty(std::string key, expression* value)
{
}

std::wstring Parser::PointZExpression::print(bool isLast, const std::wstring& prefix)
{
    return std::wstring();
}

std::string Parser::PointZExpression::emit()
{
    return "POINT.z";
}

std::unique_ptr<expression> Parser::PointZExpression::getAsRHS()
{
    return std::unique_ptr<PointZExpression>();
}

token_type Parser::PointZExpression::getExprType()
{
    return token_type::POINT;
}

expressionType Parser::PointZExpression::getType()
{
    return expressionType::VEC3;
}

expression* Parser::PointZExpression::getProperty(std::string key)
{
    return nullptr;
}

void Parser::PointZExpression::setProperty(std::string key, expression* value)
{
}

std::wstring Parser::PointYExpression::print(bool isLast, const std::wstring& prefix)
{
    return std::wstring();
}

std::string Parser::PointYExpression::emit()
{
    return "POINT.y";
}

std::unique_ptr<expression> Parser::PointYExpression::getAsRHS()
{
    return std::unique_ptr<PointYExpression>();
}

token_type Parser::PointYExpression::getExprType()
{
    return token_type::POINT;
}

expressionType Parser::PointYExpression::getType()
{
    return expressionType::VEC3;
}

expression* Parser::PointYExpression::getProperty(std::string key)
{
    return nullptr;
}

void Parser::PointYExpression::setProperty(std::string key, expression* value)
{
}

std::wstring Parser::AccessPropertyExpr::print(bool isLast, const std::wstring& prefix)
{
    return std::wstring();
}

std::string Parser::AccessPropertyExpr::emit()
{

    return lhs->getProperty(propertyName)->emit();
}

std::unique_ptr<expression> Parser::AccessPropertyExpr::getAsRHS()
{
    return lhs->getProperty(propertyName)->getAsRHS();
}

token_type Parser::AccessPropertyExpr::getExprType()
{
    return lhs->getProperty(propertyName)->getExprType();
}

expressionType Parser::AccessPropertyExpr::getType()
{
    return expressionType::ACCESS;
}

expression* Parser::AccessPropertyExpr::getProperty(std::string key)
{
    return lhs->getProperty(propertyName)->getProperty(key);
}

void Parser::AccessPropertyExpr::setProperty(std::string key, expression* value)
{
    lhs->getProperty(propertyName)->setProperty(key,value);
}
