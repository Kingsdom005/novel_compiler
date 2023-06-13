#ifndef _GRAMMAR_H
#define _GRAMMAR_H
#include <set>
#include <vector>
#include <iterator>
#include <map>
#include <list>
#include "token.h"
#include "cfg.h"

using namespace std;

struct				table_node
{
	int				row_pos;
	string			col_pos;
	int				val;
};

struct				item_node
{
	size_t			cfg_no;
	size_t			dot_pos;
	int				possible_prefix;
};

class my_less : less<item_node>
{
public:
	bool operator()(const item_node& a, const item_node& b)	const
	{
		return (a.cfg_no<b.cfg_no && a.dot_pos<b.dot_pos && a.possible_prefix<b.possible_prefix);
	}
};

class grammar : public readcfg
{
public:
	grammar(const string& filename);
	~grammar();
	void print_table();
	void print_tablehead();
	void print_state();
	void print_state(size_t i);
	void write_intermediate_process();
	void write_table(const string& filename);
	void write_tablehead(const string& filename);

	const set<int>& get_first(const int x);
	const set<int> get_mutifirst(const list<int>& x);
	size_t make_items(); //This is init items(states)
	vector<item_node>& get_closure(vector<item_node> &closure);
	vector<item_node> get_goto(const size_t i, const int x);

private:
	vector<item_node> item;
	vector<vector<item_node> > closure_item;
	vector<map<int, int> > action_goto;
    map<int,set<int> > first_map;
	size_t init_item(); //This is init all item before the program begin
	const set<string> init_first(const string x);
	const set<int> init_first(const int x);
	
	void init_table();
	void print_one_item(const size_t i, const vector<item_node>& itm);
	bool can_export_e(const int x);
	bool can_export_e(const string& x);
};

#endif
