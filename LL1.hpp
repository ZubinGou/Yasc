#ifndef LL1_HPP
#define LL1_HPP

#include <string>
#include <vector>
#include <set>
#include <map>

struct Grammar
{
    char start;
    std::set<char> T;                                // 终结符
    std::set<char> N;                                // 非终结符
    std::vector<std::pair<char, std::string>> prods; //产生式
};

class Parser
{
public:
    std::string input_string; // input std::string
    Parser(std::string config_file = "./grammar.json");
    void analysis(std::string input_string); // Predictive analysis

private:
    Grammar grammar;
    std::map<char, std::set<char>> first;           // first std::set
    std::map<char, std::set<char>> follow;          // follow std::set
    std::vector<char> analysis_stack;               // analysis stack
    std::map<std::pair<char, char>, std::string> M; // analysis table

    void eliminate_left_recursion();
    void get_first();
    void get_follow();
    void get_first_of_str(std::string str, std::set<char> &first_str);
    void generate_ll_table();
    char get_new_symbol(Grammar grammar);
    void print_grammar(Grammar grammar);
    void print_analysis_state(int ip, std::string output, std::string left_sentence);
    void load_grammar(std::string config_file);
};

#endif