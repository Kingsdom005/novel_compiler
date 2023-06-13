#ifndef _scanner_H
#define _scanner_H
#include <fstream>
#include <string>
#include "token.h"

using namespace std;

class scanner : public token
{
private:
	std::ifstream fin;
	std::ofstream fout;
	char c;
	int linenum;
	token digit_process();
	token alpha_process();
	token string_process();
public:
	scanner(const string& filename);
	~scanner();
	token scan();
	int get_linenum(){return linenum;};
	void writetofile(const token& tok);
	void print_token(const token& tok);
};

#endif
