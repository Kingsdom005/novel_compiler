#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <vector>
#include <list>
#include <iterator>
#include <algorithm>
#include <time.h>
#include "grammar.h"
#include "token.h"

grammar::grammar(const string& filename) : readcfg(filename)
{
}

grammar::~grammar()
{
	first_map.clear();
	item.clear();
	closure_item.clear();
	action_goto.clear();
}

bool grammar::can_export_e(const int x)
{
    int e = get_int("e");
	for (vector<cfg_node>::const_iterator p=cfg.begin(); p!=cfg.end(); p++)
	{
		if ((*p).left_int==x && (*p).right_int.size()==2 && (*(*p).right_int.begin())==e)
			return true; 
	}
	return false;
}

const set<int> grammar::init_first(const int x)
{
	set<int> first;
    int e = get_int("e");
	if (is_terminal(x))
	{
		first.insert(x);
		return first;
	}
	if (is_nonterminal(x))
	{
		for (size_t i=0; i<cfg.size(); i++)
		{
			if (cfg[i].left_int==x)
			{
				for (size_t j=0; j<cfg[i].right_int.size()-1; j++) 
				{
					if (cfg[i].right_int[j]!=x)
					{
						set<int> temp = init_first(cfg[i].right_int[j]);
						temp.erase(e);
						first.insert(temp.begin(), temp.end());
						if (!can_export_e(cfg[i].right_int[j]))
						{
							break;
						}
					}
				}
			}
		}
	}
	if (can_export_e(x))
	{
		first.insert(e);
	}
	return first;
}

const set<int>& grammar::get_first(const int x)
{
	// Init the first_map
	if (first_map.size()==0)
	{
		time_t time_s = time(NULL);
		for (size_t i=0; i<table_head.size(); i++)
		{
			first_map.insert(std::pair<int, set<int> >(table_head[i], init_first(table_head[i])));
		}
		time_t time_e = time(NULL);
		cerr << "Init first cost: " << (time_e - time_s)/60 << " minutes,"
			<< (time_e - time_s)%60 << " seconds.\n";
	}
	return first_map[x];
}

const set<int> grammar::get_mutifirst(const list<int>& x)
{
	set<int> first;
	set<int> prev_first, curr_first;
	bool flag = false;
	prev_first = curr_first = get_first(*(x.begin()));
    int e = get_int("e");
	if (prev_first.find(e)!=prev_first.end())
	{
		curr_first.erase(e);
		flag = true;
	}
	first.insert(curr_first.begin(), curr_first.end());
	list<int>::const_iterator p=x.begin();
	p = x.begin();
	p++;
	for ( ; p!=x.end() && flag; p++)
	{
		curr_first = first_map[*p];
		if (prev_first.find(e)!=prev_first.end())
		{
			prev_first = curr_first;
			curr_first.erase(e);
			first.insert(curr_first.begin(), curr_first.end());
		}
		else
			break;
	}
	return first;
}

vector<item_node>& grammar::get_closure(vector<item_node> &closure)
{
	for (size_t pos=0; pos<closure.size();pos++)
	{
		size_t cfg_no = closure[pos].cfg_no;
		size_t dot_pos = closure[pos].dot_pos;
		int prefix = closure[pos].possible_prefix;
		int non_tml = cfg[cfg_no].right_int[dot_pos];

		if (is_nonterminal(non_tml))
		{
			list<int> bataa;
			size_t dot_pos;
			for (dot_pos=closure[pos].dot_pos+1; dot_pos<cfg[cfg_no].right_int.size()-1; dot_pos++)
			{
				bataa.push_back(cfg[cfg_no].right_int[dot_pos]);
			}
			if (dot_pos==cfg[cfg_no].right_int.size()-1)
				bataa.push_back(prefix);
			set<int> mfirst = get_mutifirst(bataa);

			if (mfirst.size()>0)
			{
				for (size_t j=0; j<cfg.size(); j++)
				{
					if (cfg[j].left_int==non_tml)
					{
						set<int>::const_iterator p_first;
						for (p_first=mfirst.begin(); p_first!=mfirst.end(); p_first++)
						{
							int first_temp;
							int e = get_int("e");
							first_temp = ((*p_first==e) ? ERROR : (*p_first));
							size_t k;
							size_t size=closure.size();
							for (k=0; k<size/2+1; k++)
							{
								if ((closure[k].cfg_no==j && closure[k].dot_pos==0
									&& closure[k].possible_prefix==first_temp) ||
									((closure[size-k-1].cfg_no==j && closure[size-k-1].dot_pos==0
									&& closure[size-k-1].possible_prefix==first_temp)))
									break;
							}
							if (k==size/2+1)
							{
								item_node* item_temp = new item_node;
								item_temp->cfg_no = j;
								item_temp->dot_pos = 0;
								item_temp->possible_prefix = first_temp;
								closure.push_back(*item_temp);
								delete item_temp;
							}
						} //end if
					}
				}//end for
			} // end if
		} //end  if
	} //end for

	return closure;
}

vector<item_node> grammar::get_goto(const size_t i, const int x)
{
	vector<item_node> j;
	static const int dollar = get_int("$");
	for (size_t k=0; k<closure_item[i].size(); k++)
	{
		size_t cfg_no = closure_item[i][k].cfg_no;
		size_t dot_pos = closure_item[i][k].dot_pos;
		int prefix = closure_item[i][k].possible_prefix;
		if (cfg[cfg_no].right_int.size()>dot_pos && cfg[cfg_no].right_int[dot_pos]==x && x!=dollar)
		{
			item_node* p_item = new item_node;
			p_item->possible_prefix = prefix;
			p_item->cfg_no = cfg_no;
			p_item->dot_pos = dot_pos+1;
			j.push_back(*p_item);
			delete p_item;
		}
	}
	return get_closure(j);
}

size_t grammar::init_item()
{
	for (size_t i=0; i<cfg.size(); i++)
	{
		for (size_t j=0; j<cfg[i].right.size(); j++)
		{
			for (set<int>::const_iterator p_terminal=terminal.begin(); p_terminal!=terminal.end(); p_terminal++)
			{
				item_node item_temp;
				item_temp.cfg_no = i;
				item_temp.dot_pos = j;
				item_temp.possible_prefix = *p_terminal;
				item.push_back(item_temp);
			}
		}
	}
	return item.size();
}

size_t grammar::make_items()
{
	init_item();
	vector<item_node> item_tmp;
	item_node* tmp = new item_node;
	const int dollar = get_int("$");
	tmp->possible_prefix = dollar;
	tmp->cfg_no = 0;
	tmp->dot_pos = 0;
	item_tmp.push_back(*tmp);

	time_t time_s = time(NULL);
	cerr << "\nInit items begin...\n";
	closure_item.push_back(get_closure(item_tmp));
	static size_t count = 0;
	for (size_t i=0; i<closure_item.size(); i++)
	{
		set<int> set_temp;
		for (size_t j=0; j<closure_item[i].size(); j++) 
		{
			vector<item_node> goto_tmp;
			int prefix = closure_item[i][j].possible_prefix; 
			size_t cfg_no = closure_item[i][j].cfg_no;
			size_t dot_pos = closure_item[i][j].dot_pos;
			int str_nxdot = cfg[cfg_no].right_int[dot_pos];

			if (str_nxdot!=dollar && set_temp.find(str_nxdot)==set_temp.end())
			{
				set_temp.insert(str_nxdot);
				goto_tmp = get_goto(i, str_nxdot);
			}
			else
			{
				//2. R(j) begin-
				if (cfg_no!=0 && dot_pos==cfg[cfg_no].right.size()-1)
				{
					if (action_goto[i][prefix]==ERROR)
					{
						action_goto[i][prefix] = -(int)cfg_no;
					}
					else
					{
						cout << "Reduction conflict state: " << i << endl;;
					}
				} 
				//3. Accept---
				if (cfg_no==0 && prefix==dollar && dot_pos==cfg[cfg_no].right.size()-1)
				{
					if (action_goto[i][dollar]==token::ERROR)
						action_goto[i][dollar] = token::ACC;
					else
					{
						cerr << "Accept conflict\n";
					}
				}
				//end---
				continue;
			}
			if (goto_tmp.size()>0)
			{
				map<int, int> head;
				for (size_t line=0; line<table_head.size(); line++) 
				{
					head[table_head[line]] = ERROR;
				}
				action_goto.push_back(head);

				bool already_in = false;
				for (int k=closure_item.size()-1; k>=0 && (!already_in); k--)
				{
					if (closure_item[k].size()==goto_tmp.size())
					{
						size_t m;
						size_t mx_size = goto_tmp.size();
						for (m=0; m<mx_size/2+1; m++)
						{
							if (   closure_item[k][m].cfg_no!=goto_tmp[m].cfg_no
								|| closure_item[k][m].dot_pos!=goto_tmp[m].dot_pos
								|| closure_item[k][m].possible_prefix!=goto_tmp[m].possible_prefix
								|| closure_item[k][mx_size-m-1].cfg_no!=goto_tmp[mx_size-m-1].cfg_no
								|| closure_item[k][mx_size-m-1].dot_pos!=goto_tmp[mx_size-m-1].dot_pos
								|| closure_item[k][mx_size-m-1].possible_prefix!=goto_tmp[mx_size-m-1].possible_prefix)
							{
								break;
							}
						}
						if (m==mx_size/2+1)
						{
							already_in = true;
							//1. S(j) begin ---
							if (action_goto[i][str_nxdot]==token::ERROR)
								action_goto[i][str_nxdot] = k;
							else
							{
								cerr << "Shift in conflict\n";
							}
							//1. S(j) end --
							break;
						}
					}
				}
				if (!already_in)
				{
					closure_item.push_back(goto_tmp);
					//1. S(j) begin --
					if (action_goto[i][str_nxdot]==token::ERROR)
						action_goto[i][str_nxdot] = closure_item.size()-1;
					else
					{
						cerr << "Shift(goto_table) in conflict\n";
					}
					//1. S(j) end ---
				} //end if
			} //end if
		} //end for
	} //end for
	delete tmp;
	time_t time_e = time(NULL);
	cerr << "Init items end. Cost " <<  (time_e-time_s)/60 << " minutes "
		<< (time_e-time_s)%60 << " seconds.\n"
		<< "Total state: " << closure_item.size() << endl << endl;
	return closure_item.size();
}

void grammar::print_table()
{
	cout << "\t";
	for (size_t i=0; i<table_head.size(); i++)
	{
		cout << table_head[i] << "\t";
	}
	cout << endl;
	cout << "\t";
	for (size_t i=0; i<table_head.size(); i++)
	{
		cout << string_table_head[i] << "\t";
	}
	cout << endl;
	cout << "--------------------------------------------------------------------\n";
	for (size_t i=0; i<closure_item.size(); i++)
	{
		cout << i << "\t";
		for (size_t j=0; j<table_head.size(); j++)
		{
			if (action_goto[i][table_head[j]]==token::ACC)
				cout << "ACC" << "\t";
			else if (action_goto[i][table_head[j]]==token::ERROR)
				cout << "ERROR" << "\t";
			else
				cout << action_goto[i][table_head[j]] << "\t";
		}
		cout << endl;
	}
}


void grammar::print_one_item(size_t i, const vector<item_node>& itm)
{
	cout << "I" << i << ":\n";
	for (vector<item_node>::const_iterator p=itm.begin(); p!=itm.end(); p++)
	{
		print_gramma((*p).cfg_no, (*p).dot_pos);
		cout << "\t, " << (*p).possible_prefix<< endl;
	}
}

void grammar::print_state()
{
	for (size_t i=0; i<closure_item.size(); i++)
	{
		print_one_item(i, closure_item[i]);
	}
}

void grammar::print_state(size_t i)
{
	print_one_item(i, closure_item[i]);
}

void grammar::write_table(const string& filename)
{
	ofstream fout;
	fout.open(filename.c_str(), ios::app);
	if (!fout.is_open())
	{
		cerr << "Can't open "<<filename << endl;
		exit(-1);
	}
	//write the row, col
	fout << closure_item.size() << ' ' << table_head.size()<< endl;
	for (size_t i=0; i<table_head.size(); i++)
	{
		fout << table_head[i] << ' ';
	}
	fout << endl;

	for (size_t i=0; i<closure_item.size(); i++)
	{
		for (size_t j=0; j<table_head.size(); j++)
		{
			fout << action_goto[i][table_head[j]] << "\t";
		}
		fout << endl;
	}

}

void grammar::write_tablehead(const string& filename)
{
	ofstream fout;
	fout.open(filename.c_str());
	if (!fout.is_open())
	{
		cerr << "Open " << filename << " failed.\n";
		return;
	}
	fout.close();
}

void grammar::write_intermediate_process(){
	ofstream fout;
	fout.open("grammar.txt");
	
	//first set
	fout<<"firtst set:"<<endl;
	for(auto pair: first_map){
		fout<<pair.first<<":";
		for(auto first : pair.second){
			fout<<first<<" ";
		}
		fout<<endl;
	}
	fout<<endl;
	fout<<"action / go table"<<endl;
	write_table("grammar.txt");
}
