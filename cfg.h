#ifndef _CFG_H
#define _CFG_H
#include <set>
#include <vector>
#include <iterator>
#include <map>
#include <list>
#include "token.h"

using namespace std;

struct				cfg_node
{
	string			left;
	int             left_int;
	vector<string>	right;
	vector<int>     right_int;
};

class readcfg : public token
{
public:
	readcfg(const string& filename);
	~readcfg();
	bool is_terminal(const int x);
	bool is_nonterminal(const int x);
	void print_gramma(const size_t i);
	void print_gramma(const size_t i, const size_t dpos);
	void print_gramma();
protected:
	vector<cfg_node> cfg;
	vector<int> table_head;
	vector<string> string_table_head;
	set<int>	terminal;
	set<int>	nonterminal;
private:
	void init_grammar(const string& filename);
	void init_variable();
};

#endif 