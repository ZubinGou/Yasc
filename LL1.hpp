#ifndef LL1_HPP
#define LL1_HPP

#include <map>
#include <set>
#include <string>
#include <vector>

using namespace std;

struct Grammar {
    char start;                        //开始符号
    set<char> T;                       // 终结符
    set<char> N;                       // 非终结符
    vector<pair<char, string>> prods;  //产生式
};

class Parser {
   public:
    string input_string;  // input string
    Parser(string config_file = "./grammar.json");
    void analysis(string input_string);  // Predictive analysis

   private:
    Grammar grammar;
    map<char, set<char>> first;       // first set
    map<char, set<char>> follow;      // follow set
    vector<char> analysis_stack;      // analysis stack
    map<pair<char, char>, string> M;  // analysis table

    void eliminate_left_recursion();
    void get_first();
    void get_follow();
    void get_first_of_str(string str, set<char> &first_str);
    void generate_ll_table();
    char get_new_symbol(Grammar grammar);
    void print_grammar(Grammar grammar);
    void print_analysis_state(int ip, string output, string left_sentence);
    void load_grammar(string config_file);
};

#endif