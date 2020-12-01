#ifndef LL1_HPP
#define LL1_HPP

#include <map>
#include <queue>
#include <set>
#include <string>
#include <vector>

using namespace std;
struct Grammar {
    char start;
    set<char> T;                       // 终结符
    set<char> N;                       // 非终结符
    vector<pair<char, string>> prods;  //产生式
};

struct Item  // LR1 item
{
    pair<char, string> prod;  // production
    int dot;                  // dot position
    char lookahead;           // lookahead string
    bool operator<(const Item& rhs) const {
        if (prod.first < rhs.prod.first)
            return true;
        else if (prod.first > rhs.prod.first)
            return false;
        else if (prod.second < rhs.prod.second)
            return true;
        else if (prod.second > rhs.prod.second)
            return false;
        else if (dot < rhs.dot)
            return true;
        else if (dot > rhs.dot)
            return false;
        else
            return lookahead < rhs.lookahead;
    }
    bool operator==(const Item& rhs) const {
        return prod == rhs.prod && dot == rhs.dot && lookahead == rhs.lookahead;
    }
};

// typedef set<Item> Items;  //  LR1 item set

class Parser {
   public:
    string input_string;  // input string
    Parser(string config_file = "./grammar.json");
    void analysis(string input_string);  // Predictive analysis

   private:
    Grammar grammar;
    map<char, set<char>> first;              // first set
    vector<set<Item>> canonical_collection;  // canonical collection
    vector<map<char, int>> dfa;              // dfa
    vector<char> analysis_stack;             // analysis stack
    map<pair<int, char>, pair<char, int>> table_action;
    map<pair<int, char>, int> table_goto;

    void expand_grammar();
    void get_first();
    void get_first_of_str(string str, set<char>& first_str);
    void closure(set<Item>& I);
    set<Item> go(set<Item> I, char x);
    int find_items_in_cc(set<Item> items);
    void get_dfa();
    void generate_lr_table();
    char get_new_symbol(Grammar grammar);
    void print_items(set<Item>);
    void print_grammar(Grammar grammar);
    void print_analysis_state(int ip, string output, string left_sentence);
    void load_grammar(string config_file);
};

#endif