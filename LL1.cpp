#include "LL1.hpp"

#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "json.hpp"
#include "lexer.hpp"

#define COL_WIDTH 20  // analysis output width

using namespace std;
using json = nlohmann::json;

void Parser::eliminate_left_recursion() {
    map<char, vector<string>> prod_map;
    for (auto const &item : grammar.prods) {
        if (prod_map.count(item.first))
            prod_map[item.first].push_back(item.second);
        else
            prod_map[item.first] = {item.second};
    }

    for (const auto &item : prod_map) {
        char k = item.first;
        vector<string> v = item.second;
        vector<string> left_rec;
        for (const auto &rhs : v) {
            if (rhs[0] == k)
                left_rec.push_back(rhs);
        }
        if (!left_rec.empty()) {
            // delete old production
            for (auto iter = grammar.prods.begin(); iter != grammar.prods.end(); iter++) {
                if (iter->first == k)
                    grammar.prods.erase(iter--);
            }
            char new_symbol = get_new_symbol(grammar);
            grammar.N.insert(new_symbol);
            for (auto rhs : v) {
                if (rhs[0] != k)
                    grammar.prods.push_back(make_pair(k, rhs + new_symbol));
            }
            for (auto rhs : left_rec) {
                grammar.prods.push_back(make_pair(new_symbol, rhs.substr(1) + new_symbol));
            }
            grammar.prods.push_back(make_pair(new_symbol, "#"));
        }
    }
}

void Parser::get_first() {
    // First(T) = T
    for (auto const &lhs : grammar.T) {
        first[lhs].insert(lhs);
    }
    bool updated = true;
    while (updated) {
        updated = false;
        for (auto &prod : grammar.prods) {
            char lhs = prod.first;
            string rhs = prod.second;
            if (grammar.T.count(rhs[0]) || rhs[0] == '#') {
                if (!first[lhs].count(rhs[0])) {
                    updated = true;
                    first[lhs].insert(rhs[0]);
                }
            } else {
                for (auto const c : rhs) {
                    for (auto const item : first[c]) {
                        if (item != '#' && !first[lhs].count(item)) {
                            first[lhs].insert(item);
                            updated = true;
                        }
                    }
                    if (!first[c].count('#'))
                        break;
                }
            }
        }
    }
    // ouput FIRST
    printf("FIRST:\n");
    for (auto const &lhs : grammar.N) {
        printf("    %c: ", lhs);
        for (auto item : first[lhs]) {
            printf("%c ", item);
        }
        printf("\n");
    }
    // for (auto const &lhs : grammar.T)
    // {
    //     printf("%c: ", lhs);
    //     for (auto it = first[lhs].begin(); it != first[lhs].end(); it++)
    //     {
    //         printf("%c ", *it);
    //     }
    //     printf("\n");
    // }
}

void Parser::get_follow() {
    follow[grammar.start].insert('$');
    bool updated = true;
    while (updated) {
        updated = false;
        for (auto &prod : grammar.prods) {
            char lhs = prod.first;
            string rhs = prod.second;
            for (int i = 0; i < rhs.length(); i++) {
                char B = rhs[i];
                if (!grammar.N.count(B))
                    continue;
                string beta = rhs.substr(i + 1);
                set<char> first_beta;  // fisrt set of str
                get_first_of_str(beta, first_beta);

                for (auto item : first_beta) {
                    if (item != '#' && !follow[B].count(item)) {
                        follow[B].insert(item);
                        updated = true;
                    }
                }
                // A->aB or A->aBb b->epsilon, add follow(A) to follow(B)
                if ((i + 1) == rhs.length() || first_beta.count('#')) {
                    for (auto const item : follow[lhs]) {
                        if (!follow[B].count(item)) {
                            follow[B].insert(item);
                            updated = true;
                        }
                    }
                }
            }
        }
    }
    // output FOLLOW
    printf("FOLLOW:\n");
    for (auto const &lhs : grammar.N) {
        printf("    %c: ", lhs);
        for (auto item : follow[lhs]) {
            printf("%c ", item);
        }
        printf("\n");
    }
}

void Parser::get_first_of_str(string str, set<char> &first_str) {
    // if c->epsilon, next = true
    bool next = true;
    for (auto const &c : str) {
        if (!next)
            break;
        next = false;
        if (grammar.T.count(c) || c == '#') {
            if (!first_str.count(c))
                first_str.insert(c);
        } else {
            for (auto item : first[c]) {
                if (item == '#') {
                    next = true;
                } else {
                    if (!first_str.count(item))
                        first_str.insert(item);
                }
            }
        }
    }
    // str -> epsilon
    if (next) {
        first_str.insert('#');
    }
}

void Parser::generate_ll_table() {
    M.clear();  // clear old table
    for (const auto prod : grammar.prods) {
        //  A->alpha
        set<char> first_alpha;
        get_first_of_str(prod.second, first_alpha);
        for (const auto item : first_alpha) {
            M[make_pair(prod.first, item)] = string(1, prod.first) + "->" + prod.second;
        }
        // epsilon in FISRT(alpha)
        if (first_alpha.count('#')) {
            for (const auto item : follow[prod.first]) {
                M[make_pair(prod.first, item)] = string(1, prod.first) + "->" + prod.second;
            }
        }
    }

    printf("\n-----------------------------LL(1) Table-----------------------\n");
    printf("\t");
    for (auto const &symbol : grammar.T) {
        printf("%c\t", symbol);
    }
    printf("\n");

    for (const auto &A : grammar.N) {
        printf("%c\t", A);
        for (const auto &a : grammar.T) {
            for (auto k : M[make_pair(A, a)]) {
                printf("%c", k);
            }
            printf("\t");
        }
        printf("\n");
    }
    printf("\n-----------------------Init Grammar Success--------------------\n");
}

char Parser::get_new_symbol(Grammar grammar) {
    for (int i = 'A'; i <= 'Z'; i++) {
        if (!grammar.N.count(i))
            return i;
    }
    return '!';
}

void Parser::print_grammar(Grammar grammar) {
    printf("Start symbol:\t\t%c\n", grammar.start);
    printf("Terminal symbol:\t");
    for (auto const &item : grammar.T)
        printf("%c ", item);

    printf("\nNon-terminal symbol:\t");
    for (auto const &item : grammar.N)
        printf("%c ", item);
    printf("\n\nProductions:\n");
    for (auto const &item : grammar.prods) {
        printf("    %c -> %s\n", item.first, item.second.c_str());
    }
    printf("\n");
}

void Parser::print_analysis_state(int ip, string output, string left_sentence) {
    std::stringstream stack_info;
    stack_info << "[" << analysis_stack[0];
    for (int i = 1; i < analysis_stack.size(); i++)
        stack_info << ", " << analysis_stack[i];
    stack_info << "]";
    cout << left << setw(COL_WIDTH * 2) << stack_info.str()
         << setw(COL_WIDTH) << input_string.substr(ip)
         << setw(COL_WIDTH) << output
         << setw(COL_WIDTH) << left_sentence << endl;
}

void Parser::load_grammar(string config_file) {
    ifstream grammar_config(config_file);
    json j;
    grammar_config >> j;

    grammar.start = j["start"].get<string>()[0];

    for (const auto &item : j["N"])
        grammar.N.insert(item.get<string>()[0]);

    for (const auto &item : j["T"])
        grammar.T.insert(item.get<string>()[0]);

    for (const auto &item : j["productions"].items())
        for (const auto &val : item.value())
            grammar.prods.push_back(make_pair(item.key()[0], val.get<string>()));

    grammar.T.insert('$');
}

void Parser::analysis(string in_str) {
    input_string = in_str + "$";
    // init stack
    analysis_stack.clear();
    analysis_stack.push_back('$');
    analysis_stack.push_back(grammar.start);
    int ip = 0;
    // @top : top of stack
    // @in : current input char
    char top = analysis_stack.back(), in;
    string left_sentence = string(1, grammar.start);
    cout << left << setw(COL_WIDTH * 2) << "\nStack"
         << setw(COL_WIDTH) << "Input"
         << setw(COL_WIDTH) << "Output"
         << setw(COL_WIDTH) << "Left Sentence" << endl;
    print_analysis_state(ip, "", left_sentence);
    while (top != '$') {
        in = input_string[ip];
        // X is T or $
        if (grammar.T.count(top)) {
            // if match, pop and forward
            if (top == in) {
                analysis_stack.pop_back();
                ip++;
                print_analysis_state(ip, "", left_sentence);
            } else {
                printf("Error: input not match!\n");
                break;
            }
        } else  // X is N
        {
            string s = M[make_pair(top, in)];
            if (!s.empty()) {
                analysis_stack.pop_back();
                for (int i = s.size() - 1; i >= 3; i--) {
                    if (s[i] != '#')
                        analysis_stack.push_back(s[i]);
                }
                size_t pos = left_sentence.find(s[0]);
                string rhs = s.substr(3);
                if (rhs == "#")
                    rhs = "";
                left_sentence.replace(pos, 1, rhs);
                print_analysis_state(ip, s, left_sentence);
            } else {
                printf("Error: M[%c][%c] is empty.\n", top, in);
                break;
            }
        }
        top = analysis_stack.back();
    }
    printf("--------------------------------------------------------------------------\n");
}

Parser::Parser(string config_file) {
    load_grammar(config_file);

    printf("----------------Raw Grammar---------------\n");
    print_grammar(grammar);

    // Eliminate left recursion
    eliminate_left_recursion();
    printf("---------Eliminate Left Recursion---------\n");
    print_grammar(grammar);

    // Get first and follow set
    printf("--------------FISRT and FOLLOW------------\n");
    get_first();
    get_follow();

    // generat ll(1) table
    generate_ll_table();
}

// driver
int main() {
    Lexer lexer;

    Parser parser("./grammar.json");

    cout << "Input: ";
    while (cin.getline(lexer.src, 100))  // ctrl-z to stop
    {
        string tokenStream = lexer.getTokenStream();
        cout << "Token stream:" << tokenStream << endl;
        parser.analysis(tokenStream);
        cout << "Input: ";
    }
    return 0;
}