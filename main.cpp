#define NOMINMAX
#include "lexer.h"    // 1. Your Project Headers
#include "parser.h"

#include <iostream>   // 2. C++ Standard Library
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

#include <io.h>       // 3. C/System Specific Headers (Non-Standard)
#include <fcntl.h>
int main(){
    tokenizer t;
    Parser p;
   
    std::ifstream f("file.txt");
    std::stringstream buffer;
    buffer << f.rdbuf();
    std::cout << buffer.str();
    std::vector<token> x=t.tokenize(buffer.str());
    for(token i :x){
        std::cout << i << "\n";
    }
    int ptr = 0;
    _setmode(_fileno(stdout), _O_U16TEXT);
    std::unique_ptr<statement> smt= p.parseProgram(x);
    std::wstring result = smt->print(true, L"");
    std::string code = smt->emit();

    std::wcout <<L"\n" << result << L"\n";
    std::wcout << L"\n" << std::wstring(code.begin(), code.end()) << L"\n";
}