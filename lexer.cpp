#include "lexer.hpp"

// map punctuation value to type
map<char, int> punctuation2token = {
    {',', Comma,},
    {':', Colon,},
    {';', Semicolon},
    {'?', Question},  
    {'(', Brace_l},
    {')', Brace_r},
    {'[', Square_l},
    {']', Square_r},
    {'{', Parenthesis_l},
    {'}', Parenthesis_r}, 
};

// map token type to output and value
map<int, pair<string, string>> token_map = {
    {Unknown, make_pair("UNKNOWN", "")},
    {Sys, make_pair("SYS_FUNCTION", "")},
    {Preprocessor, make_pair("PREPROCESSOR", "")},
    {Line_comment, make_pair("LINE_COMMENT", "IGNORED")},
    {Block_comment, make_pair("BLOCK_COMMENT", "IGNORED")},

    {Identifier, make_pair("IDNETIFIER", "")},
    {Integer, make_pair("INTEGER", "")},
    {Floating, make_pair("FLOATING", "")},
    {Character, make_pair("CHARACTER", "")},
    {String_literal, make_pair("STRING_LITERAL", "")},

    {Auto, make_pair("AUTO", "auto")},
    {Break, make_pair("BREAK", "break")},
    {Case, make_pair("CASE", "case")},
    {Char, make_pair("CHAR", "char")},
    {Const, make_pair("CONST", "const")},
    {Continue, make_pair("CONTINUE", "continue")},
    {Default, make_pair("DEFAULT", "default")},
    {Do, make_pair("DO", "do")},
    {Double, make_pair("DOUBLE", "double")},
    {Else, make_pair("ELSE", "else")},
    {Enum, make_pair("ENUM", "enum")},
    {Extern, make_pair("EXTERN", "extern")},
    {Float, make_pair("FLOAT", "float")},
    {For, make_pair("FOR", "for")},
    {Goto, make_pair("GOTO", "goto")},
    {If, make_pair("IF", "if")},
    {Inline, make_pair("INLINE", "inline")},
    {Int, make_pair("INT", "int")},
    {Long, make_pair("LONG", "long")},
    {Register, make_pair("REGISTER", "register")},
    {Restrict, make_pair("RESTRICT", "restrict")},
    {Return, make_pair("RETURN", "return")},
    {Short, make_pair("SHORT", "short")},
    {Signed, make_pair("SIGNED", "signed")},
    {Sizeof, make_pair("SIZEOF", "sizeof")},
    {Static, make_pair("STATIC", "static")},
    {Struct, make_pair("STRUCT", "struct")},
    {Switch, make_pair("SWITCH", "switch")},
    {Typedef, make_pair("TYPEDEF", "typedef")},
    {Union, make_pair("UNION", "union")},
    {Unsigned, make_pair("UNSIGNED", "unsigned")},
    {Void, make_pair("VOID", "void")},
    {Volatile, make_pair("VOLATILE", "volatile")},
    {While, make_pair("WHILE", "while")},

    {Assign, make_pair("Assign", "=")},
    {Add, make_pair("ADD", "+")},
    {Inc, make_pair("INC", "++")},
    {Sub, make_pair("SUB", "-")},
    {Dec, make_pair("DEC", "--")},
    {Div, make_pair("DIV", "/")},
    {Mod, make_pair("MOD", "%")},
    {Bitwise_or, make_pair("BITWISE_OR", "|")},
    {Bitwise_xor, make_pair("BITWISE_XOR", "^")},
    {Bitwise_not, make_pair("BITWISE_NOT", "~")},
    {And, make_pair("AND", "&&")},
    {Or, make_pair("OR", "||")},
    {Not, make_pair("NOT", "!")},
    {Shl, make_pair("SHL", "<<")},
    {Shr, make_pair("SHR", ">>")},
    {Equal, make_pair("EQUAL", "==")},
    {Inequal, make_pair("INEQUAL", "!=")},
    {Less, make_pair("LESS", "<")},
    {Less_equal, make_pair("LESS_EQUAL", "<=")},
    {Greater, make_pair("GREATER", ">")},
    {Greater_equal, make_pair("GREATER_EQUAL", ">=")},
    {Comma, make_pair("COMMA", ",")},
    {Colon, make_pair("COLON", ":")},
    {Semicolon, make_pair("SEMICOLON", ";")},
    {Dot, make_pair("DOT", ".")},
    {Asterisk, make_pair("ASTERISK", "*")},
    {Ampersand, make_pair("AMPERSAND", "&")},
    {Question, make_pair("QUESTION", "?")},
    {Arrow, make_pair("ARROW", "->")},
    {Concat, make_pair("CONCAT", "##")},
    {Brace_l, make_pair("BRACE_L", "(")},
    {Brace_r, make_pair("BRACE_R", ")")},
    {Square_l, make_pair("SQUARE_L", "[")},
    {Square_r, make_pair("SQUARE_R", "]")},
    {Parenthesis_l, make_pair("PARENTHESIS_L", "{")},
    {Parenthesis_r, make_pair("PARENTHESIS_R", "}")},
    {Add_assign, make_pair("ADD_ASSIGN", "+=")},
    {Sub_assign, make_pair("SUB_ASSIGN", "-=")},
    {Mul_assign, make_pair("MUL_ASSIGN", "*=")},
    {Div_assign, make_pair("DIV_ASSIGN", "/=")},
    {Mod_assign, make_pair("MOD_ASSIGN", "%=")},
    {Bitwise_and_assign, make_pair("BITWISE_AND_ASSIGN", "&=")},
    {Bitwise_or_assign, make_pair("BITWISE_OR_ASSIGN", "|=")},
    {Bitwise_xor_assign, make_pair("BITWISE_XOR_ASSIGN", "^=")},
    {And_assign, make_pair("AND_ASSIGN", "&&=")},
    {Or_assign, make_pair("OR_ASSIGN", "||=")},
    {Shl_assign, make_pair("SHL_ASSIGN", "<<=")},
    {Shr_assign, make_pair("SHR_ASSIGN", ">>=")}
};

long long Lexer::getDigit()
{
    long long digit = *src - '0';
    src++;
    if (digit > 0)
    {
        // dec, starts with [1-9]
        while (*src >= '0' && *src <= '9')
        {
            digit = digit * 10 + *src++ - '0';
        }
    }
    else
    {
        // starts with number 0
        if (*src == 'x' || *src == 'X')
        {
            //hex
            src++;
            while ((*src >= '0' && *src <= '9') || (*src >= 'a' && *src <= 'f') || (*src >= 'A' && *src <= 'F'))
            {
                digit = digit * 16 + (*src & 15) + (*src >= 'A' ? 9 : 0);
                src++;
            }
        }
        else
        {
            // oct
            while (*src >= '0' && *src <= '7')
            {
                digit = digit * 8 + *src++ - '0';
            }
        }
    }
    return digit;
}

void Lexer::next()
{
    char *begin_pos;
    int hash;
    column_now = src - src_col_begin + 1;
    while (token = *src)
    {
        ++src;

        if (token == '\n')
        {
            ++row_count;
            src_col_begin = src;
            column_now = 1;
        }
        else if (token == '#')
        {
            // skip macro, because we will not support it
            if (*src == '#')
            {
                src++;
                token = Concat;
            }
            else
            {
                begin_pos = data;
                *data++ = token;
                while (*src != 0 && *src != '\n')
                {
                    *data++ = *src++;
                }
                *data++ = '\0';
                token = Preprocessor;
                token_val = (long long)begin_pos;
            }
            return;
        }
        else if (token == '/')
        {
            if (*src == '/')
            {
                // line comment
                while (*src != 0 && *src != '\n')
                {
                    ++src;
                }
                token = Line_comment;
            }
            else if (*src == '*')
            {
                // block comment
                src++;
                while (*src != 0)
                {
                    token = *src++;
                    if (token == '\n')
                    {
                        src_col_begin = src;
                        row_count++;
                    }
                    if (token == '*' && *src == '/')
                    {
                        src++;
                        break;
                    }
                }
                if (*src == 0)
                {
                    printf(">>>> %d:%d Error:  Block comment unclosed [Not Fixed]\n",
                           row_count, column_now, begin_pos);
                }
                token = Block_comment;
            }
            else if (*src == '=')
            {
                src++;
                token = Div_assign;
            }
            else
            {
                // divide operator
                token = Div;
            }
            return;
        }
        else if ((token >= 'a' && token <= 'z') || (token >= 'A' && token <= 'Z') || (token == '_'))
        {
            // parse identifier
            begin_pos = data;
            *data++ = token;
            hash = token;

            while ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z') || (*src >= '0' && *src <= '9') || (*src == '_'))
            {
                *data++ = *src;
                hash = hash * 147 + *src;
                src++;
            }
            *data++ = '\0';

            // look for existing identifier, linear search
            current_id = symbols;
            while (current_id[Token])
            {
                if (current_id[Hash] == hash &&
                    !memcmp((char *)current_id[Name], begin_pos, data - begin_pos))
                {
                    //found one, return
                    token = current_id[Token];
                    return;
                }
                current_id = current_id + IdSize;
            }

            // store new ID
            current_id[Name] = (long long)begin_pos; //TODO
            current_id[Hash] = hash;
            token = current_id[Token] = Identifier;
            return;
        }
        else if (token >= '0' && token <= '9' || token == '.')
        {
            // parse number, three kinds: dec(123) hex(0x123) oct(017)
            char *src_num_begin = --src;
            if (token == '.')
            {
                token_val = 0;
            }
            else
            {
                token_val = getDigit();
            }
            if (*src != '.' && *src != 'e' && *src != 'E')
            {
                token = Integer;
                // ignore suffix of integer
                if (*src == 'u' || *src == 'U')
                {
                    src++;
                    if (*src == 'l' || *src == 'L')
                        src++;
                    if (*src == 'l' || *src == 'L')
                        src++;
                }
                if (*src == 'l' || *src == 'L')
                {
                    src++;
                    if (*src == 'l' || *src == 'L')
                        src++;
                    if (*src == 'u' || *src == 'U')
                        src++;
                }
            }
            else
            {
                token = Floating;
                token_val_double = (double)token_val;
                if (*src == '.')
                {
                    src++;
                    double frac = 1;
                    while (*src >= '0' && *src <= '9')
                    {
                        frac *= 0.1;
                        token_val_double += frac * (*src++ - '0');
                    }
                }

                if (*src == 'e' || *src == 'E')
                {
                    src++;
                    int sign = 1;
                    if (*src == '+' || *src == '-')
                    {
                        if (*src == '-')
                            sign = -1;
                        src++;
                    }
                    if (*src >= '0' && *src <= '9')
                    {
                        long long digit = sign * getDigit();
                        token_val_double *= pow(10, digit);
                    }
                    else
                    {
                        column_now = src - src_col_begin + 1;
                        printf(">>>> %d:%d Error:  Incomplete number\n\t\t\"",
                               row_count, column_now);
                        for (; src_num_begin < src; src_num_begin++)
                            putchar(*src_num_begin);
                        printf("\" [Audo Fixed]\n");
                    }
                }
                if (*src == 'f' || *src == 'F' || *src == 'l' || *src == 'L')
                {
                    // ignore suffix of floating
                    src++;
                }
            }
            int unexpected_suffix = 0;
            while (isalnum(*src))
            {
                src++;
                unexpected_suffix = 1;
            }
            if (unexpected_suffix)
            {
                column_now = src - src_col_begin + 1;
                printf(">>>> %d:%d Error:  Unexpected number suffix\n\t\t\"",
                       row_count, column_now);
                for (; src_num_begin < src; src_num_begin++)
                    putchar(*src_num_begin);
                printf("\" [Audo Fixed]\n");
            }
            return;
        }
        else if (token == '\'')
        {
            // parse character
            begin_pos = data;
            token_val = *src++;
            if (token_val == '\\')
            {
                token_val = *src++;
                *data++ = '\\';
            }
            *data++ = token_val;
            *data++ = '\0';

            if (*src == '\'')
            {
                src++;
            }
            else
            {
                column_now = src - src_col_begin + 1;
                printf(">>>> %d:%d Error:  Char unclosed\n\t\t\"'%s\" [Auto Fixed]\n",
                       row_count, column_now, begin_pos);
            }
            token = Character;
            token_val = (long long)begin_pos;
            return;
        }
        else if (token == '"')
        {
            // parse string literal (include keywords)
            begin_pos = data;
            while (*src != 0 && *src != '"')
            {
                token_val = *src++;
                if (token_val == '\\')
                {
                    // literal escape character for output
                    token_val = *src++;
                    if (token_val == '\n')
                    {
                        token_val = *src++;
                    }
                    else
                    {
                        *data++ = '\\';
                    }
                }
                *data++ = token_val;
            }

            if (*src == '"')
            {
                token_val = (long long)begin_pos;
                token = String_literal;
            }
            else
            {
                // TODO deal with Error
                token = Unknown;
                printf(">>>> %d:%d Error:  String unclosed\n\t\t\"%s\" [Not Fixed]\n",
                       row_count, column_now, begin_pos);
            }
            src++;
            return;
        }
        else if (token == '=')
        {
            // parse '==' and '='
            if (*src == '=')
            {
                src++;
                token = Equal;
            }
            else
            {
                token = Assign;
            }
            return;
        }
        else if (token == '+')
        {
            // parse '+', '+' and '+='
            if (*src == '+')
            {
                src++;
                token = Inc;
            }
            else if (*src == '=')
            {
                src++;
                token = Add_assign;
            }
            else
            {
                token = Add;
            }
            return;
        }
        else if (token == '-')
        {
            // parse '-', '--' and '-='
            if (*src == '-')
            {
                src++;
                token = Dec;
            }
            else if (*src == '=')
            {
                src++;
                token = Sub_assign;
            }
            else if (*src == '>')
            {
                src++;
                token = Arrow;
            }
            else
            {
                token = Sub;
            }
            return;
        }
        else if (token == '%')
        {
            if (*src == '=')
            {
                src++;
                token = Mod_assign;
            }
            else
            {
                token = Mod;
            }
            return;
        }
        else if (token == '|')
        {
            // parse '|', '|' and '||='
            if (*src == '|')
            {
                src++;
                if (*src == '=')
                {
                    src++;
                    token = Or_assign;
                }
                else
                {
                    token = Or;
                }
            }
            else if (*src == '=')
            {
                token = Bitwise_or_assign;
            }
            else
            {
                token = Bitwise_or;
            }
            return;
        }
        else if (token == '^')
        {
            if (*src == '=')
            {
                src++;
                token = Bitwise_xor_assign;
            }
            else
            {
                token = Bitwise_xor;
            }
            return;
        }
        else if (token == '~')
        {
            token = Bitwise_not;
        }
        else if (token == '&')
        {
            // parse '&' and '&&' and '&='
            if (*src == '&')
            {
                src++;
                token = And;
            }
            else if (*src == '=')
            {
                src++;
                token = Bitwise_and_assign;
            }
            else
            {
                token = Ampersand;
            }
            return;
        }
        else if (token == '!')
        {
            // parse '!' and '!='
            if (*src == '=')
            {
                src++;
                token = Inequal;
            }
            else
            {
                token = Not;
            }
            return;
        }
        else if (token == '<')
        {
            // parse '<=', '<<', '<' and '<<='
            if (*src == '=')
            {
                src++;
                token = Less_equal;
            }
            else if (*src == '<')
            {
                src++;
                if (*src == '=')
                {
                    src++;
                    token = Shl_assign;
                }
                else
                {
                    token = Shl;
                }
            }
            else
            {
                token = Less;
            }
            return;
        }
        else if (token == '>')
        {
            // parse '>=', '>>', '>' and '>>='
            if (*src == '=')
            {
                src++;
                token = Greater_equal;
            }
            else if (*src == '>')
            {
                src++;
                if (*src == '=')
                {
                    src++;
                    token = Shr_assign;
                }
                else
                {
                    token = Shr;
                }
            }
            else
            {
                token = Greater;
            }
            return;
        }
        else if (token == '*')
        {
            if (*src == '=')
            {
                src++;
                token = Mul_assign;
            }
            else
            {
                token = Asterisk;
            }
            return;
        }
        else if (token == ',' || token == ':' || token == ';' || token == '?' || token == '{' || token == '}' || token == '(' || token == ')' || token == '[' || token == ']')
        {
            token = punctuation2token[token];
            return;
        }
        else if (token == ' ' || token == '\t')
        {
            column_now = src - src_col_begin + 1;
        }
        // else if (token == '@' || token == '$') {
        // }
        else
        {
            printf(">>>> %d:%d Error:  Invalid symbol %c [Auto Fixed]\n", row_count, column_now, token);
        }
    }
    return;
}

void Lexer::printCounter()
{
    int keywordCount = 0;
    int punctuatorCount = 0;
    for (int i = Auto; i <= While; i++)
    {
        keywordCount += token_count[i];
    }

    for (int i = Assign; i <= Shr_assign; i++)
    {
        punctuatorCount += token_count[i];
    }

    printf("\n\n===============Counter=================\n");
    printf("        ROW:         %d\n", row_count);
    printf("        TOTAL_CHAR:  %d\n", total_char_count);
    printf("        TOTAL_TOKEN: %d\n", total_token_count);
    printf("        KEYWORD:     %d\n", keywordCount);
    printf("        SYS_FUNC:    %d\n", token_count[Sys]);
    printf("        IDENTIFIER:  %d\n", token_count[Identifier]);
    printf("        INTEGER:     %d\n", token_count[Integer]);
    printf("        FLOATING:    %d\n", token_count[Floating]);
    printf("        CHAR:        %d\n", token_count[Character]);
    printf("        STRING:      %d\n", token_count[String_literal]);
    printf("        PUNCTUATOR:  %d\n", punctuatorCount);
    printf("        PERPROCESS:  %d\n", token_count[Preprocessor]);
    printf("        COMMENT:     %d\n", token_count[Line_comment] + token_count[Block_comment]);
    printf("=======================================\n");
    return;
}

void Lexer::lexDriver()
{
    // get next token
    next();
    while (token > 0)
    {
        total_token_count++;
        token_count[token]++;
        printf("(%3d :%3d)  %-18s=>   ", row_count, column_now, token_map[token].first.c_str());

        if (token == Integer)
        {
            printf("\"%lld\"\n", token_val);
        }
        else if (token == Floating)
        {
            printf("\"%f\"\n", token_val_double);
        }
        else if (token == Character || token == String_literal || token == Preprocessor)
        {
            printf("\"%s\"\n", (char *)token_val);
        }
        else if (token == Identifier)
        {
            printf("\"%s\"  @0X00%x \n", current_id[Name], current_id);
        }
        else if (token == Sys)
        {
            printf("\"%s\"\n", current_id[Name], current_id);
        }
        else if (token == Unknown)
        {
            printf("\"UNKNOWN\"\n");
        }
        else
        {
            printf("\"%s\"\n", token_map[token].second.c_str());
        }

        next();
    }
}

void Lexer::printHelpInfo()
{
    printf("Clexer: C lexer written in C++\n");
    printf("Usage:\n\tclex [-d] <file>\n");
    printf("Flags:\n\t-h\t\t Print help information\n");
    printf("\t-d\t\t Use debug mode\n");
}

Lexer::Lexer()
{
    // #define int long long // to work with 64bit address
    debug = 0;
    // printHelpInfo();

    mem_size = 256 * 1024; // arbitrary size

    // allocate memory
    if (!(data = (char *)malloc(mem_size)))
    {
        printf("could not malloc(%d) for data area\n", mem_size);
        return;
    }
    if (!(symbols = (int *)malloc(mem_size)))
    {
        printf("could not malloc(%d) for symbol table\n", mem_size);
        return;
    }

    memset(data, 0, mem_size);
    memset(symbols, 0, mem_size);

    src = (char *)"auto break case char const continue default do double else enum \
    extern float for goto if inline int long register restrict return short signed \
    sizeof static struct switch typedef union unsigned void volatile while \
    open read close printf malloc memset memcmp exit void main";

    // add keywords to symbol table
    int i = Auto;
    while (i <= While)
    {
        next();
        current_id[Token] = i++;
    }

    while (token > 0)
    {
        next();
        current_id[Token] = Sys;
    }

    if (!(src_col_begin = src = (char *)malloc(mem_size)))
    {
        printf("could not malloc(%d) for source area\n", mem_size);
        return;
    }
}

void Lexer::parse_file(string src_file)
{
    FILE *fd;
    if ((fd = fopen(src_file.c_str(), "r")) == NULL)
    {
        printf("could not open(%s)\n", src_file.c_str());
        return;
    }
    // read the source file
    int i;
    if ((i = fread(src, 1, mem_size - 1, fd)) <= 0)
    {
        printf("read() returned %d\n", i);
        return;
    }
    src[i] = 0; // add EOF character
    fclose(fd);

    column_now = 1; // current column
    row_count = 1;  // row_count number of source code
    total_char_count = 0;
    total_token_count = 0;
    memset(token_count, 0, sizeof(token_count));

    char *src_begin = src;
    lexDriver();
    total_char_count = src - src_begin;

    printCounter();
}

string Lexer::getTokenStream()
{
    string tokenStream = "";
    next();
    while (token > 0) {
        if (token == Integer || token == Floating)
            tokenStream += "n";
        else
            tokenStream += token_map[token].second;
        next();
    }
    return tokenStream;
}