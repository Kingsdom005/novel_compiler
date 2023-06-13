#include <iostream>
#include <fstream>
#include "cfg.h"
#include <cstdlib>

readcfg::readcfg(const string& filename)
{
	init_grammar(filename);
	init_variable();
}
readcfg::~readcfg()
{
	for (size_t i=0; i<cfg.size(); i++)
		cfg[i].right.clear();
	cfg.clear();
	terminal.clear();
	nonterminal.clear();
}

void readcfg::init_grammar(const string& filename)
{
	ifstream fin;
	fin.open(filename.c_str());
	if(!fin.is_open())
	{
		cerr << "Can't open " << filename << endl;
		exit(-1);
	}

	string pre_word, cur_word;
	//Init the CFG
	cfg_node* p_temp = new cfg_node;
	fin >> pre_word;
	fin >> cur_word;
	while (cur_word.size()!=0)
	{
		if (cur_word=="->")
		{
			p_temp->left = pre_word;
			pre_word.clear();
			pre_word = cur_word;
			cur_word.clear();
			fin >> cur_word;

			while (cur_word.size()!=0 && cur_word!="->")
			{
				if (pre_word!="->")
				{
					p_temp->right.push_back(pre_word);
					p_temp->right_int.push_back(get_int(pre_word));
				}
				pre_word.clear();
				pre_word = cur_word;
				cur_word.clear();
				fin >> cur_word;
			}
		}
		if (cur_word.size()==0)
		{
			p_temp->right.push_back(pre_word);
		}
		p_temp->right.push_back("$");
		cfg.push_back(*p_temp);
		p_temp->left.clear();
		p_temp->right.clear();
	}
	delete p_temp;

	fin.close();
}

void readcfg::init_variable()
{
	set<string> t, nt;
	//Init the terminal and nonterminal table
	for (size_t i=0; i<cfg.size(); i++)
	{
		nt.insert(cfg[i].left);
	}
	t.insert("$");
	for (size_t i=0; i<cfg.size(); i++)
	{
		for (size_t j=0; j<cfg[i].right.size()-1; j++)
		{
			if (nt.find(cfg[i].right[j])==nt.end())
			{
				string* s = new string(cfg[i].right[j]);
				t.insert(*s);
				delete s;
			}
		}
	}
	set<string>::iterator p;
	for (size_t i=0; i<cfg.size(); i++)
	{
		cfg[i].left_int = get_int(cfg[i].left);
		cfg[i].right_int.clear();
		for (size_t j=0; j<cfg[i].right.size(); j++)
		{
			cfg[i].right_int.push_back(get_int(cfg[i].right[j]));
		}
	}

	for (p=t.begin(); p!=t.end(); p++)
	{
		int temp=get_int(*p);
		terminal.insert(temp);
		table_head.push_back(temp);
		string_table_head.push_back(*p);
	}
	for (p=nt.begin(); p!=nt.end(); p++)
	{
		int temp=get_int(*p);
		nonterminal.insert(temp);
		table_head.push_back(temp);
		string_table_head.push_back(*p);
	}
}

bool readcfg::is_terminal(const int x)
{
	return terminal.find(x)!=terminal.end();
}

bool readcfg::is_nonterminal(const int x)
{
	return nonterminal.find(x)!=nonterminal.end();
}

void readcfg::print_gramma(const size_t i)
{
	cout << cfg[i].left << " -> ";
	for (size_t j=0; j<cfg[i].right.size()-1; j++)
		cout << cfg[i].right[j] << ' ';
	cout << '\t';
	cout << cfg[i].left_int << " -> ";
	for (size_t j=0; j<cfg[i].right_int.size()-1; j++)
		cout << cfg[i].right_int[j] << ' ';
	cout << endl;
}

void readcfg::print_gramma(const size_t i, const size_t dpos)
{
	cout << cfg[i].left << " -> ";
	for (size_t j=0; j<cfg[i].right.size(); j++)
	{
		if (dpos==j)
			cout << ".";
		if (j<cfg[i].right.size()-1)
			cout << cfg[i].right[j] << ' ';
	}
}

void readcfg::print_gramma()
{
	for (size_t i=0; i<cfg.size(); i++)
	{
		cout << i << ": ";
		print_gramma(i);
	}
}
