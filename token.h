#ifndef _TOKEN_H
#define _TOKEN_H
#include <string>
#include <map>
#include <iostream>

using namespace std;

class token
{
private:
	void init_hash();
	map<string, int> hash_table;
public:
	enum table
	{
		NAME_SIZE = 256,
		OP_SIZE = 57,
		ERROR = 1000,
		ACC = 2000,
		FILE_END = 3000,
	};
	token(const int& t, const string& v);
	token(const string& v);
	token();
	~token(){};
	int tag;
	string val;
	int get_int(const string& s);
};

#endif
