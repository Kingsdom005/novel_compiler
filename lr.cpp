#include <iostream>
#include <stack>
#include <string>
#include <fstream>
#include "lr.h"
#include "token.h"
#include "scanner.h"
#include <cstdlib>

string setNum(int x)
{
	string temp;
	while (x)
	{
		char c = x%10 + '0';
		temp = c + temp;
		x /=10;
	}
	return temp;
}

int Temp::addrCount=0;

Temp::Temp()
{
}

int Temp::newTempAddr()
{
	return (++addrCount);
}


lr::lr(const string& cfgfile, const string& table_file):readcfg(cfgfile)
{
	nextInstr = 1;
	init_table(table_file);
}
lr::~lr()
{
}

void lr::init_table(const string& filename)
{
    std::ifstream fin;
	fin.open(filename.c_str());
	if (!fin.is_open())
	{
		cerr << "Can't open "<<filename << endl;
		exit(-1);
	}
    size_t row=0, col=0;
    fin >> row >> col;
	row_size = row;
	col_size = col;

	string temp;
	getline(fin, temp);
	getline(fin, temp);
	for (size_t i=0; i<row_size; i++)
	{
	    int x=token::ERROR;
	    vector<int> oneline;
		for (size_t j=0; j<col_size; j++)
		{
            fin >> x;
            oneline.push_back(x);
		}
		action_goto.push_back(oneline);
	}
	fin.close();
}

void lr::print_table()
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
	cout << "----------------------------------------------------------------------\n";
	for (size_t i=0; i<row_size; i++)
	{
		cout << i << "\t";
		for (size_t j=0; j<col_size; j++)
		{
			if (action_goto[i][j]==token::ACC)
				cout << "ACC" << "\t";
			else if (action_goto[i][j]==token::ERROR)
				cout << "ERROR" << "\t";
			else
				cout << action_goto[i][j] << "\t";
		}
		cout << endl;
	}
}


bool lr::analyse(const string& filename)
{
	
	stack<stack_node>stk;
	vector<string> input_token;
	int len;
	stack_node* p_node = new stack_node;
	token curr_tok;
	p_node->state = 0;
	scanner s(filename);
	p_node->state = 0;
	p_node->tok.val = "$";
	p_node->tok.tag = get_int("$");
	stk.push(*p_node);

	unsigned pos=0, count=0;
	stack_node stack_top;
	curr_tok = s.scan();

	

	while (!stk.empty())
	{
		stack_top = stk.top();
		int k, k2;
		
		if (stack_top.state>=0 && (size_t)stack_top.state<action_goto.size() 
			&& (size_t)curr_tok.tag<=action_goto[0].size()
			&& curr_tok.tag>0)
		{
		    
			k = action_goto[stack_top.state][curr_tok.tag-1];
			k2 = action_goto[stack_top.state][get_int("e")-1];
		}
		else
		{
			cerr << "line " << s.get_linenum() << ", illegal character \'" << curr_tok.val << "\'\n";
			curr_tok = s.scan();
			stk.pop();
			continue;
		}
		if ((k>0 && k!=ERROR && k!=ACC) || (k2>0 && k2!=ERROR && k2!=ACC))
		{
			if ((k>0 && k!=ERROR && k!=ACC))
				p_node->state = k;
			else
				p_node->state = k2;
			p_node->tok = curr_tok;
			p_node->nodeInfo.name = p_node->tok.val;

			//Êä³ötoken
			s.print_token(p_node->tok);

			stk.push(*p_node);
			
			if ((k>0 && k!=ERROR && k!=ACC))
			{
				curr_tok = s.scan();
				if (curr_tok.tag==FILE_END)
				{
					curr_tok.tag = get_int("$");
					curr_tok.val = "$";
				}
			}
		}
		else if (k<=0 && k!=token::ERROR && k!=ACC)
		{
			vector<stack_node> rightTemp;
			token tokTemp;
			len = cfg[-k].right.size()-1;
			for (int i=0; i<len; i++)
			{
				rightTemp.push_back(stk.top());
				stk.pop();
			}
			stack_top = stk.top();
			p_node->tok.val = cfg[-k].left;
			p_node->tok.tag = cfg[-k].left_int;
			p_node->state = action_goto[stack_top.state][cfg[-k].left_int-1];
			
			//Êä³ö
			//s.print_token(p_node->tok);

			switch (-k)
			{
			case 0:	
				p_node->nodeInfo = rightTemp[0].nodeInfo;
				break;
			case 1:
				p_node->nodeInfo = rightTemp[0].nodeInfo;
				p_node->nodeInfo.code += gen("halt");
				break;
			case 2:
				p_node->nodeInfo = rightTemp[0].nodeInfo;
				break;
			case 3:
				p_node->nodeInfo = rightTemp[0].nodeInfo;
				break;
			case 4:
				p_node->nodeInfo = rightTemp[0].nodeInfo;
				p_node->nodeInfo.code = rightTemp[2].nodeInfo.code;
				if (!hasKey(idTable,rightTemp[1].nodeInfo.name))
				{
					idTable[rightTemp[1].nodeInfo.name].addr = idTemp.newTempAddr();
					idTable[rightTemp[1].nodeInfo.name].type = rightTemp[2].nodeInfo.type;
				}
				break;
			case 5:		
				p_node->nodeInfo.code.clear();
				p_node->nodeInfo.type = IS_VOID;
				p_node->nodeInfo.addr = rightTemp[0].nodeInfo.addr;
				break;
			case 6:		
				p_node->nodeInfo.code.clear();
				p_node->nodeInfo.type = IS_INT;
				p_node->nodeInfo.addr = rightTemp[0].nodeInfo.addr;
				break;
			case 7:		
				p_node->nodeInfo.code.clear();
				p_node->nodeInfo.type = IS_DOUBLE;
				p_node->nodeInfo.addr = rightTemp[0].nodeInfo.addr;
				break;
			case 8:		
				p_node->nodeInfo = rightTemp[2].nodeInfo;
				p_node->nodeInfo.code = rightTemp[3].nodeInfo.code + rightTemp[2].nodeInfo.code
					+ rightTemp[1].nodeInfo.code + rightTemp[0].nodeInfo.code;
				break;
			case 9:	
				if (!hasKey(idTable,rightTemp[0].nodeInfo.name))
				{
					idTable[rightTemp[0].nodeInfo.name].addr = idTemp.newTempAddr();
					idTable[rightTemp[0].nodeInfo.name].type = IS_STRING;
				}
				p_node->nodeInfo = rightTemp[0].nodeInfo;
				p_node->nodeInfo.type = IS_STRING;
				break;
			case 10:
				p_node->nodeInfo.code.clear();
				break;
			case 11:
				p_node->nodeInfo = rightTemp[1].nodeInfo;
				break;
			case 12:	
				p_node->nodeInfo = rightTemp[1].nodeInfo;
				break;
			case 13:	
				p_node->nodeInfo.code = rightTemp[2].nodeInfo.code + rightTemp[1].nodeInfo.code;
				break;
			case 14:	
				p_node->nodeInfo = rightTemp[0].nodeInfo;
				break;
			case 15:	
				p_node->nodeInfo.code = rightTemp[1].nodeInfo.code + rightTemp[0].nodeInfo.code;
				break;
			case 16:	
				p_node->nodeInfo = rightTemp[0].nodeInfo;
				break;
			case 17:	
				p_node->nodeInfo.code = rightTemp[1].nodeInfo.code + rightTemp[0].nodeInfo.code;
				break;
			case 18:
				p_node->nodeInfo = rightTemp[0].nodeInfo;
				break;
			case 19:
				p_node->nodeInfo = rightTemp[0].nodeInfo;
				break;
			case 20:	
				p_node->nodeInfo = rightTemp[1].nodeInfo;
				p_node->nodeInfo.code = backpatch(rightTemp[1].nodeInfo.code, rightTemp[1].nodeInfo.nextlist, rightTemp[0].nodeInfo.instr, nextInstr);
				break;
			case 21:
				p_node->nodeInfo = rightTemp[1].nodeInfo;
				p_node->nodeInfo.code = backpatch(rightTemp[1].nodeInfo.code, rightTemp[1].nodeInfo.nextlist, rightTemp[0].nodeInfo.instr, nextInstr);
				break;
			case 22:
				p_node->nodeInfo = rightTemp[0].nodeInfo;
				break;
			case 23:
				p_node->nodeInfo.code.clear();
				break;
			case 24:
				p_node->nodeInfo = rightTemp[1].nodeInfo;
				break;
			case 25:	
				p_node->nodeInfo = rightTemp[0].nodeInfo;
				break;
			case 26:	
				p_node->nodeInfo.addr = idTemp.newTempAddr();
				p_node->nodeInfo.code = rightTemp[2].nodeInfo.code + rightTemp[0].nodeInfo.code
					+ gen(p_node->nodeInfo.addr, "=", rightTemp[2].nodeInfo.addr, "=", rightTemp[0].nodeInfo.addr);
				p_node->nodeInfo.nextlist.push_back(nextInstr);
				break;
			case 27:
				p_node->nodeInfo = rightTemp[0].nodeInfo;
				break;
			case 28:
				p_node->nodeInfo.addr = idTemp.newTempAddr();
				p_node->nodeInfo.truelist = merge(rightTemp[3].nodeInfo.truelist, rightTemp[0].nodeInfo.truelist);
				p_node->nodeInfo.falselist = rightTemp[0].nodeInfo.falselist;
				p_node->nodeInfo.nextlist.push_back(nextInstr);
				p_node->nodeInfo.code = rightTemp[3].nodeInfo.code + rightTemp[0].nodeInfo.code;
				p_node->nodeInfo.code = backpatch(p_node->nodeInfo.code, rightTemp[3].nodeInfo.falselist, rightTemp[1].nodeInfo.instr, nextInstr);
				break;
			case 29:
				p_node->nodeInfo = rightTemp[0].nodeInfo;
				break;
			case 30:
				p_node->nodeInfo.addr = idTemp.newTempAddr();
				p_node->nodeInfo.truelist = rightTemp[0].nodeInfo.truelist;
				p_node->nodeInfo.falselist = merge(rightTemp[3].nodeInfo.falselist, rightTemp[0].nodeInfo.falselist);
				p_node->nodeInfo.nextlist.push_back(nextInstr);
				p_node->nodeInfo.code = rightTemp[3].nodeInfo.code + rightTemp[0].nodeInfo.code;
				p_node->nodeInfo.code = backpatch(p_node->nodeInfo.code, rightTemp[3].nodeInfo.truelist, rightTemp[1].nodeInfo.instr, nextInstr);
				break;
			case 31:
				p_node->nodeInfo = rightTemp[0].nodeInfo;
				break;
			case 32:
				p_node->nodeInfo.addr = idTemp.newTempAddr();
				p_node->nodeInfo.truelist.push_back(nextInstr);
				p_node->nodeInfo.falselist.push_back(nextInstr+1);
				p_node->nodeInfo.code = rightTemp[2].nodeInfo.code + rightTemp[0].nodeInfo.code
					+ gen("goto", rightTemp[2].nodeInfo.addr, "==", rightTemp[0].nodeInfo.addr);
				p_node->nodeInfo.code += gen("goto");
				break;
			case 33:
				p_node->nodeInfo = rightTemp[0].nodeInfo;
				break;		
			case 34:
				p_node->nodeInfo.addr = idTemp.newTempAddr();
				p_node->nodeInfo.truelist.push_back(nextInstr);
				p_node->nodeInfo.falselist.push_back(nextInstr+1);
				p_node->nodeInfo.code = rightTemp[2].nodeInfo.code + rightTemp[0].nodeInfo.code
					+ gen("goto", rightTemp[2].nodeInfo.addr,"<", rightTemp[0].nodeInfo.addr);
				p_node->nodeInfo.code += gen("goto");
				break;
			case 35:
				p_node->nodeInfo.addr = idTemp.newTempAddr();
				p_node->nodeInfo.truelist.push_back(nextInstr);
				p_node->nodeInfo.falselist.push_back(nextInstr+1);
				p_node->nodeInfo.code = rightTemp[2].nodeInfo.code + rightTemp[0].nodeInfo.code
					+ gen("goto", rightTemp[2].nodeInfo.addr, ">" ,rightTemp[0].nodeInfo.addr);
				p_node->nodeInfo.code += gen("goto");
				break;
			case 36:
				p_node->nodeInfo.addr = idTemp.newTempAddr();
				p_node->nodeInfo.truelist.push_back(nextInstr);
				p_node->nodeInfo.falselist.push_back(nextInstr+1);
				p_node->nodeInfo.code = rightTemp[2].nodeInfo.code + rightTemp[0].nodeInfo.code
					+ gen("goto", rightTemp[2].nodeInfo.addr, "<=", rightTemp[0].nodeInfo.addr);
				p_node->nodeInfo.code += gen("goto");
				break;
			case 37:
				p_node->nodeInfo.addr = idTemp.newTempAddr();
				p_node->nodeInfo.truelist.push_back(nextInstr);
				p_node->nodeInfo.falselist.push_back(nextInstr+1);
				p_node->nodeInfo.code = rightTemp[2].nodeInfo.code + rightTemp[0].nodeInfo.code
					+ gen("goto", rightTemp[2].nodeInfo.addr, ">=", rightTemp[0].nodeInfo.addr);
				p_node->nodeInfo.code += gen("goto");
				break;
			case 38:
				p_node->nodeInfo = rightTemp[0].nodeInfo;
				break;
			case 39:
				p_node->nodeInfo.addr = idTemp.newTempAddr();
				p_node->nodeInfo.code = rightTemp[2].nodeInfo.code + rightTemp[0].nodeInfo.code
					+ gen(p_node->nodeInfo.addr, "=", rightTemp[2].nodeInfo.addr, "-", rightTemp[0].nodeInfo.addr);
				p_node->nodeInfo.nextlist.push_back(nextInstr);
				p_node->nodeInfo.truelist.clear();
				p_node->nodeInfo.falselist.clear();
				break;
			case 40:
				p_node->nodeInfo.addr = idTemp.newTempAddr();
				p_node->nodeInfo.code = rightTemp[2].nodeInfo.code + rightTemp[0].nodeInfo.code
					+ gen(p_node->nodeInfo.addr, "=", rightTemp[2].nodeInfo.addr, "+", rightTemp[0].nodeInfo.addr);
				p_node->nodeInfo.nextlist.push_back(nextInstr);
				p_node->nodeInfo.truelist.clear();
				p_node->nodeInfo.falselist.clear();
				break;
			case 41:
				p_node->nodeInfo = rightTemp[0].nodeInfo;
				break;
			case 42:
				p_node->nodeInfo.addr = idTemp.newTempAddr();
				p_node->nodeInfo.code = rightTemp[2].nodeInfo.code + rightTemp[0].nodeInfo.code
					+ gen(p_node->nodeInfo.addr, "=", rightTemp[2].nodeInfo.addr, "*", rightTemp[0].nodeInfo.addr);
				p_node->nodeInfo.nextlist.push_back(nextInstr);
				p_node->nodeInfo.truelist.clear();
				p_node->nodeInfo.falselist.clear();
				break;
			case 43:
				p_node->nodeInfo.addr = idTemp.newTempAddr();
				p_node->nodeInfo.code = rightTemp[2].nodeInfo.code + rightTemp[0].nodeInfo.code
					+ gen(p_node->nodeInfo.addr, "=", rightTemp[2].nodeInfo.addr, "/", rightTemp[0].nodeInfo.addr);
				p_node->nodeInfo.nextlist.push_back(nextInstr);
				p_node->nodeInfo.truelist.clear();
				p_node->nodeInfo.falselist.clear();
				break;
			case 44:
				p_node->nodeInfo.addr = idTemp.newTempAddr();
				p_node->nodeInfo.code = rightTemp[2].nodeInfo.code + rightTemp[0].nodeInfo.code
					+ gen(p_node->nodeInfo.addr, "=", rightTemp[2].nodeInfo.addr, "%", rightTemp[0].nodeInfo.addr);
				p_node->nodeInfo.nextlist.push_back(nextInstr);
				p_node->nodeInfo.truelist.clear();
				p_node->nodeInfo.falselist.clear();
				break;
			case 45:
				p_node->nodeInfo = rightTemp[0].nodeInfo;
				break;
			case 46:
				if (!hasKey(idTable,rightTemp[0].nodeInfo.name))
				{
					cerr << "line:" << s.get_linenum() << ", " << rightTemp[0].nodeInfo.name << " undefine\n";
				}
				p_node->nodeInfo = rightTemp[0].nodeInfo;
				p_node->nodeInfo.addr = idTable[rightTemp[0].nodeInfo.name].addr;
				p_node->nodeInfo.nextlist.push_back(nextInstr);
				break;
			case 47:
				if (!hasKey(idTable,rightTemp[0].nodeInfo.name))
				{
					idTable[rightTemp[0].nodeInfo.name].addr = idTemp.newTempAddr();
					idTable[rightTemp[0].nodeInfo.name].type = IS_DOUBLE;
				}
				p_node->nodeInfo = rightTemp[0].nodeInfo;
				p_node->nodeInfo.addr = idTable[rightTemp[0].nodeInfo.name].addr;
				p_node->nodeInfo.nextlist.push_back(nextInstr);
				break;
			case 48:
				if (!hasKey(idTable,rightTemp[0].nodeInfo.name))
				{
					idTable[rightTemp[0].nodeInfo.name].addr = idTemp.newTempAddr();
					idTable[rightTemp[0].nodeInfo.name].type = IS_INT;
				}
				p_node->nodeInfo = rightTemp[0].nodeInfo;
				p_node->nodeInfo.addr = idTable[rightTemp[0].nodeInfo.name].addr;
				p_node->nodeInfo.nextlist.push_back(nextInstr);
				break;
			case 49:
				p_node->nodeInfo = rightTemp[1].nodeInfo;
				break;
			case 50:
				p_node->nodeInfo.nextlist = merge(rightTemp[3].nodeInfo.falselist, rightTemp[0].nodeInfo.nextlist);
				p_node->nodeInfo.code = rightTemp[3].nodeInfo.code + rightTemp[0].nodeInfo.code;
				p_node->nodeInfo.code = backpatch(p_node->nodeInfo.code, rightTemp[3].nodeInfo.truelist, rightTemp[1].nodeInfo.instr, nextInstr);
				break;
			case 51:
				p_node->nodeInfo.code = rightTemp[3].nodeInfo.code + rightTemp[0].nodeInfo.code 
					+ gen("goto", rightTemp[1].nodeInfo.instr);
				p_node->nodeInfo.code = backpatch(p_node->nodeInfo.code, rightTemp[0].nodeInfo.nextlist, rightTemp[4].nodeInfo.instr, nextInstr);
				p_node->nodeInfo.code = backpatch(p_node->nodeInfo.code, rightTemp[3].nodeInfo.truelist, rightTemp[1].nodeInfo.instr, nextInstr);
				p_node->nodeInfo.nextlist = rightTemp[3].nodeInfo.falselist;
				break;
			case 52:
				p_node->nodeInfo = rightTemp[1].nodeInfo;
				break;
			case 53:
				p_node->nodeInfo = rightTemp[1].nodeInfo;
				break;
			case 54:
				p_node->nodeInfo = rightTemp[1].nodeInfo;
				break;
			case 55:
				p_node->nodeInfo = rightTemp[1].nodeInfo;
				break;
			case 56:
				p_node->nodeInfo.instr = nextInstr;
				break;
			default:
				break;
			}
			stk.push(*p_node);
			p_node->nodeInfo.code.clear();
			p_node->nodeInfo.truelist.clear();
			p_node->nodeInfo.falselist.clear();
			p_node->nodeInfo.nextlist.clear();
		}
		else if (k==token::ACC)
		{
			cout << "\n\n" << stack_top.nodeInfo.code;
			delete p_node;
			return true;
		}
		else
		{
			delete p_node;
			return false;
		}
	}
	delete p_node;
	return false;
}


void lr::print_error()
{
	cout << "Can't receive, error\n";
}

int lr::getNextInstr()
{
	return (nextInstr++);
}

string lr::gen(const int addr, const string& equ, const int addr1, const string& op, const int addr2)
{
	int currAddr = getNextInstr();
	string temp;
	temp = setNum(currAddr) + ":\t" + op + "\t" + setNum(addr1) + "\t" + setNum(addr2) + "\t"+ setNum(addr) + "\n";
	return temp;
}

string lr::gen(const string& equ, const int addr1, const string& op, const int addr2)
{
	int currAddr = getNextInstr();
	string temp;
	temp = setNum(currAddr) + ":\tif  " + setNum(addr1) + " " + op + " " + setNum(addr2) + "\tgoto -" + "\n";
	return temp;
}

string lr::gen(const string& equ)
{
	int currAddr = getNextInstr();
	string temp;
	if (equ=="goto")
		temp =  setNum(currAddr) + ":\tgoto -\n";
	else if (equ=="halt")
		temp = setNum(currAddr) + ":\thalt";
	return temp;
}

string lr::gen(const string& equ, int index)
{
	int currAddr = getNextInstr();
	string temp;
	temp =  setNum(currAddr) + ":\tgoto " + setNum(index) + "\n";
	return temp;
}

string& lr::backpatch(string& code, vector<int> index, int instr, int end)
{
	int npos=end;
	if (index.size()==0)
		return code;
	for (size_t j=0; j<index.size(); j++)
	{
		int i = int(code.size()-1);
		for (; i>=0; i--)
		{
			char c=code[i];
			if (code[i]=='\n')
				end--;
			if (end==index[j] && code[i]=='-')
				break;
		}
		if (i>=0)
			code = code.replace(i, 1, setNum(instr));
	}
	return code;
}

vector<int> lr::merge(vector<int>& alist, vector<int>& blist)
{
	for (size_t i=0; i<blist.size(); i++)
	{
		size_t j;
		for (j=0; j<alist.size(); j++)
		{
			if (alist[j]==blist[i])
				break;
		}
		if (j==alist.size())
			alist.push_back(blist[i]);
	}
	return alist;
}

bool lr::hasKey(const map<string, idenfierNode>& m, const string& key)
{
	std::map<string, idenfierNode>::const_iterator p = m.begin();
	for (; p!=m.end(); p++)
	{
		if ((*p).first == key)
			return true;
	}
	return false;
}
