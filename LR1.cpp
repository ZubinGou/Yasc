#include "LR1.hpp"

#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "json.hpp"
#define COL_WIDTH 20  // analysis output width

using namespace std;
using json = nlohmann::json;

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

void Parser::generate_lr_table() {
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
            // for (const auto item : follow[prod.first])
            // {
            //     M[make_pair(prod.first, item)] = string(1, prod.first) + "->" + prod.second;
            // }
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

void Parser::expand_grammar() {
    grammar.prods.insert(grammar.prods.begin(), make_pair('Z', "S#"));
}

void Parser::closure(set<Item> &I) {
    bool updated = true;
    while (updated) {
        updated = false;
        set<Item> J(I);
        /* 枚举每个项目 */
        // print_items(J);
        for (auto item : J) {
            if (item.dot < item.prod.second.length()) {
                char B = item.prod.second[item.dot];
                if (grammar.N.count(B)) {
                    set<char> first_ba;
                    string ba = item.prod.second.substr(item.dot + 1) + item.lookahead;
                    get_first_of_str(ba, first_ba);
                    for (auto p : grammar.prods) {
                        if (p.first == B) {
                            for (auto t : first_ba) {
                                Item i;
                                i.prod = p;
                                i.lookahead = t;
                                i.dot = 0;
                                if (I.find(i) == I.end()) {
                                    I.insert(i);
                                    updated = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
set<Item> Parser::go(set<Item> I, char x) {
    set<Item> next_items;
    for (auto item : I) {
        if (item.dot < item.prod.second.length()) {
            if (x == item.prod.second[item.dot]) {
                Item i = item;
                i.dot++;
                next_items.insert(i);
            }
        }
    }
    closure(next_items);
    printf("Items: ");
    print_items(I);
    printf("next te:");
    print_items(next_items);
    return next_items;
}

int Parser::find_items_in_cc(set<Item> items) {
    for (int i = 0; i < canonical_collection.size(); i++) {
        if (canonical_collection[i] == items)
            return i;
    }
    return -1;
}

void Parser::print_items(set<Item> items) {
    for (auto item : items) {
        printf("%c->", item.prod.first);
        for (int j = 0; j < item.prod.second.length(); j++) {
            if (item.dot == j)
                printf(".");
            printf("%c", item.prod.second[j]);
        }
        if (item.dot == item.prod.second.length())
            printf(".");
        printf(", %c   ", item.lookahead);
    }
    printf("\n");
}

void Parser::get_dfa() {
    // init items
    Item i;
    i.lookahead = '#';
    i.dot = 0;
    i.prod.first = 'Z';
    i.prod.second = string(1, grammar.start);

    set<Item> I;
    I.insert(i);
    closure(I);
    
    canonical_collection.push_back(I);

    queue<pair<int, set<Item>>> remain_items;
    remain_items.push(make_pair(0, I));
    while (!remain_items.empty()) {
        int idx = remain_items.front().first;
        set<Item> cur_items = remain_items.front().second;
        remain_items.pop();
        set<char> TN;
        set_union(grammar.T.begin(), grammar.T.end(), grammar.N.begin(), grammar.N.end(),
                  insert_iterator<set<char>>(TN, TN.begin()));
        for (auto t : TN) {  // every Terminal sysmbol
            set<Item> next_items = go(cur_items, t);
            if (!next_items.empty()) {
                int next_idx = find_items_in_cc(next_items);
                if (next_idx == -1) {  // items not in cc
                    next_idx = canonical_collection.size();
                    canonical_collection.push_back(next_items);
                    remain_items.push(make_pair(next_idx, next_items));
                }
                dfa[idx][t] = next_idx;
            }
        }
    }

    printf("Canonical Collection Size: %d\n", (int)canonical_collection.size());
    for (int i = 0; i < canonical_collection.size(); i++) {
        printf("I%d:\n", i);
        // print items
        print_items(canonical_collection[i]);
        // print dfa
        for (auto m : dfa[i]) {
            printf("\n--%c-->I%d", m.first, m.second);
        }
        printf("\n\n");
    }
}

Parser::Parser(string config_file) {
    load_grammar(config_file);
    printf("----------------Raw Grammar---------------\n");
    print_grammar(grammar);

    expand_grammar();

    // Get first set
    printf("-----------------FISRT Set----------------\n");
    get_first();

    // Get Canonical Collection and DFA
    printf("---------Canonical Collection & DFA-------\n");
    get_dfa();

    // generat lr(1) table
    generate_lr_table();
}

int main() {
    Parser parser;
}