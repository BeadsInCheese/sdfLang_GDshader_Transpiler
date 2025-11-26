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
    return identifier;
}
std::wstring Parser::NumberExpression::print(bool isLast, const std::wstring& prefix)
{
    return prefix+ L"└── "+std::to_wstring(number) + L"\n";
}
std::string Parser::NumberExpression::emit()
{
    return std::to_string(number);
}
std::wstring expression::print(bool isLast, const std::wstring& prefix)
{
    return prefix+ L"└── default expression. Some error has happened during parsing\n";
}


Parser::NumberExpression::NumberExpression(std::string value) {
    number = std::stof(value);
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
    else if (look(tokens, ptr).typ == token_type::IDENTIFIER) {
        return parseIdentifierExpression(tokens,ptr);

    }
    else if (look(tokens, ptr).typ == token_type::POINT) {
        consume(tokens,ptr);
        return std::make_unique<IdentifierExpression>("POINT",variables.get());

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
    std::string w=consume(tokens, ptr).val;
    std::wcout << std::wstring(w.begin(),w.end());
    assignment->lhs = parseExpression(tokens, ptr,-1);
    token t = expect(tokens, ptr, token_type::ASSIGN);
    assignment->rhs = parseExpression(tokens, ptr,-1);
    expect(tokens, ptr, token_type::SEMICOLON);
    return std::move(assignment);
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
    return std::string("");
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
    return rhs->emit()+"="+lhs->emit()+";";
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
    return "return " + returnValue->emit() + ";";
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
    std::wstring out = prefix + (isLast ? L"└── " : L"├── ") + L"declare" + L"\n";

    std::wstring childPrefix = prefix + (isLast ? L"    " : L"│   ");
    out += lhs->print(true, childPrefix);
    out += rhs->print(true, childPrefix);
    return out;
}

std::string Parser::declarationStatement::emit()
{
    NamedValue n;
    n.type_information = type_information.typ;
    n.value = std::make_unique<expression>(*rhs.get());
    n.position[0] = 0.0f; 
    n.position[1] = 0.0f;
    n.position[2] = 0.0f;

    n.rotation[0] = 0.0f;
    n.rotation[1] = 0.0f;
    n.rotation[2] = 0.0f;

    n.scale[0] = 0.0f;
    n.scale[0] = 0.0f;
    n.scale[0] = 0.0f;
    (*variables)[lhs->identifier] = std::move(n);
    return std::string("");
};


