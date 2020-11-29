#include "lexer.hpp"
#include <iostream>

using namespace std;

Lexer lexer;
void match(int tk);
double procE();
double procEend(double lvalue);
double procT();
double procTend(double lvalue);
double procF();

void match(int tk)
{
    if (lexer.token == tk)
    {
       lexer.next();
    }
    else
    {
        printf(">>>> (%d :%d) ", lexer.row_count, lexer.column_now);
        printf("Expected lexer.token: `%s`, but get `%s`\n", token_map[tk].first.c_str(), token_map[lexer.token].first.c_str());
    }
}

double procE()
{
    double lvalue = procT();
    return procEend(lvalue);
}

double procEend(double lvalue)
{
    if (lexer.token == Add)
    {
        match(Add);
        double value = lvalue + procT();
        return procEend(value);
    }
    else if (lexer.token == Sub)
    {
        match(Sub);
        double value = lvalue - procT();
        return procEend(value);
    }
    else
    {
        return lvalue;
    }
}

double procT()
{
    double lvalue = procF();
    return procTend(lvalue);
}

double procTend(double lvalue)
{
    if (lexer.token == Asterisk)
    {
        match(Asterisk);
        double value = lvalue * procF();
        return procTend(value);
    }
    else if (lexer.token == Div)
    {
        match(Div);
        double divisor = procF();
        if (divisor == 0)
        {
            printf(">>>> (%d :%d) Error:  Divide by Zero.\n",
                   lexer.row_count, lexer.column_now);
            return 0;
        }
        else
        {
            double value = lvalue / divisor;
            return procTend(value);
        }
    }
    else
    {
        return lvalue;
    }
}

double procF()
{
    double value = 0;
    if (lexer.token == Brace_l)
    {
        match(Brace_l);
        value = procE();
        match(Brace_r);
    }
    else if (lexer.token == Integer)
    {
        value = lexer.token_val;
        match(Integer);
    }
    else
    {
        value = lexer.token_val_double;
        match(Floating);
    }
    return value;
}

int main()
{
    // lexer.parse_file("./test/test.c");
    while (cin.getline(lexer.src, 100)) // ctrl-z stop
    {   
        lexer.next();
        printf("=> %lf\n\n", procE());
    }
    return 0;
}