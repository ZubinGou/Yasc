#include <iostream>
#include <string>
#include "lexer.hpp"
#include "LL1.hpp"

using namespace std;

int main()
{
    Lexer lexer;

    Parser parser("./grammar.json");

    cout << "Input: ";
    while (cin.getline(lexer.src, 100)) // ctrl-z to stop
    {   
        string tokenStream = lexer.getTokenStream();
        cout << "Token stream:" << tokenStream << endl;
        parser.analysis(tokenStream);
        cout << "Input: ";
    }
    return 0;
}
