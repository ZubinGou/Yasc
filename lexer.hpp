#ifndef LEXER_HPP
#define LEXER_HPP
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <iostream>
#include <map>
#include <cmath>

using namespace std;

class Lexer
{
public:
    int debug = 0; // debug mode
    char *src;           // pointer to source code string;
    int token;           // current token
    long long token_val; // value of current token (mainly for number)
    double token_val_double;

    // Counter
    int column_now = 0; // current column
    int row_count = 0;  // row_count number of source code
    int total_char_count = 0;
    int total_token_count = 0;
    int token_count[512] = {0};

    Lexer();
    void next();
    void parse_file(string src_file);
    string getTokenStream();

private:
    int mem_size;
    char *data;          // data segment
    char *src_col_begin; // pointer to beginning of current column
    int *current_id;     // current parsed ID
    int *symbols;        // symbol table

    long long getDigit();
    void lexDriver();
    void printHelpInfo();
    void printCounter();
};

// fields of identifier
enum {Token, Hash, Name, Value, IdSize};

// token types
enum {
    // Special
    Unknown = 128, Sys, Preprocessor, Line_comment, Block_comment,
    Identifier, Number, Integer, Floating, Character, String_literal,

    // Keywords
    Auto, Break, Case, Char, Const, Continue, Default, Do, Double, Else, Enum,
    Extern, Float, For, Goto, If, Inline, Int, Long, Register, Restrict, Return,
    Short, Signed, Sizeof, Static, Struct, Switch, Typedef, Union, Unsigned,
    Void, Volatile, While, // _Bool, _Complex, _Imaginary

    // Punctuators
    Assign, Add, Inc, Sub, Dec, Div, Mod, Bitwise_or, Bitwise_xor, Bitwise_not,
    And, Or, Not, Shl, Shr, Equal, Inequal, Less, Less_equal, Greater,
    Greater_equal, Comma, Colon, Semicolon, Dot, Asterisk, Ampersand, Question,
    Arrow, Concat, Brace_l, Brace_r, Square_l, Square_r, Parenthesis_l, 
    Parenthesis_r, Add_assign, Sub_assign, Mul_assign, Div_assign, Mod_assign,
    Bitwise_and_assign, Bitwise_or_assign, Bitwise_xor_assign, And_assign,
    Or_assign, Shl_assign, Shr_assign
} token_types;

extern map<char, int> punctuation2token;
extern map<int, pair<string, string>> token_map;

#endif