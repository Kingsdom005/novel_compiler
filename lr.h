#ifndef _LR1_H
#define _LR1_H
#include "grammar.h"
#include "cfg.h"
#include "token.h"
#include <string>
#include <map>

using namespace std;

enum
{
	IS_INT,
	IS_DOUBLE,
	IS_STRING,
	IS_VOID,
};

struct U_node
{
	string name;
	string code;
	int type;
	int addr;
	string op;
	int instr;	
	vector<int> truelist;
	vector<int> falselist;
	vector<int> nextlist;
};

struct stack_node
{
	int state;
	token tok;

	U_node nodeInfo;
};

struct code_cfg_node
{
	int left;
	vector<int> right;
};

struct idenfierNode
{
	int type;
	int addr;
};

class Temp
{
public:
	Temp();
	static int addrCount;
	int newTempAddr();

};

class lr : public readcfg
{
public:
	lr(const string& cfgfile, const string& table_file);
	~lr();
	bool analyse(const string& filename);
	void print_table();
private:
	vector<code_cfg_node> coding_cfg;
    size_t row_size;
    size_t col_size;
	vector<vector<int> > table;
	vector<vector<int> > action_goto;
	void init_table(const string& filename);
	void print_error();
	map<std::string, idenfierNode> idTable;
	Temp idTemp;
	string gen(const int addr, const string& equ, const int addr1, const string& op, const int addr2);
	string gen(const string& equ, const int addr1, const string& op, const int addr2);
	string gen(const string& equ);
	string gen(const string& equ, int index);
	string& backpatch(string& code, vector<int> index, int instr, int end);
	vector<int> merge(vector<int>& alist, vector<int>& blist);
	int nextInstr;
	int getNextInstr();
	bool hasKey(const map<string, idenfierNode>& m, const string& key);
};

#endif
