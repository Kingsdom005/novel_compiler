#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include "scanner.h"
#include "token.h"

scanner::scanner(const string& filename)
{
	using std::cerr;
	linenum = 1;
	c = ' ';
	fin.open(filename.c_str());
	if (!fin.is_open())
	{
		cerr << filename << " open failed.\n";
		exit(-1);
	}
	fout.open("temp.txt");
	if (!fout.is_open())
	{
		cerr << "temp.txt open failed.\n";
		exit(-1);
	}
}
scanner::~scanner()
{
	fin.close();
	fout.close();
}

token scanner::digit_process()
{
	int i=0;
	int float_flag = 1;
	char digit[token::NAME_SIZE] = {0};
	digit[i++] = c;
	while (isdigit(fin.peek()))
	{
		fin.get(c);
		digit[i++] = c;
	}
	if (fin.peek()=='.')
	{
		float_flag++;
		digit[i++] = c;
		while(isdigit(fin.peek()))
		{
			fin.get(c);
			digit[i++] = c;
		}
		string s(digit);
		return (token(get_int("int_num"),s));
	}
	else
	{
		string s_int(digit);
		return (token(get_int("double_num"),s_int));
	}
}

token scanner::alpha_process()
{
	char alp[NAME_SIZE] = {0};
	int i=0;
	string s;
	while((isalpha(c)) || (isdigit(c)) || c=='_')
	{
		alp[i++] = c;
		if (!fin.get(c))
			break;
	}
	if (i!=0)
		fin.putback(c);
	alp[i] = '\0';
	s.assign(alp);
	token tok_temp;
	int tag = tok_temp.get_int(s);
	if (tag==ERROR)
		return (token(get_int("id"), s));
	return (token(tag,s));
}

token scanner::string_process()
{
	char str[token::NAME_SIZE] = {0};
	int i=0;
	str[i++] = c;
	while (fin.get(c) && c!='"')
	{
		str[i++] = c;
		if (c=='\n')	linenum++;
	}
	str[i++] = c;
	string s(str);
	return (token(get_int("string"),s));
}

token scanner::scan()
{
	int i=0;
	string s;
	static char prev_c = 0;
	char line[token::NAME_SIZE] = {0};
	prev_c = c;
	while (true)
	{
		if (!fin.get(c))	return (token());
		if (c=='\n')	linenum++;
		else if (c==' ' || c=='\t')	continue;
		else	break;
	}
	while (c=='/' && (fin.peek()=='/' || fin.peek()=='*'))
	{
		if (c=='/' && fin.peek()=='/')
		{
			while(c!='\n')
				fin.get(c);
			linenum++;
			while (true)
			{
				if (!fin.get(c))
					return (token());
				if (c=='\n')	linenum++;
				else if (c==' ' || c=='\t')	continue;
				else break;
			}
		}
		if (c=='/' && fin.peek()=='*')
		{
			fin.get(c);
			fin.get(c);
			if (c=='\n') linenum++;
			while (!(c=='*' && fin.peek()=='/'))
			{
				if (!fin.get(c)) return (token());
				else if (c=='\n')	linenum++;
			}
			fin.get(c);
			fin.get(c);
			if (c=='\n') linenum++;
			while (true)
			{
				if (!fin.get(c))
					return (token());
				else if (c=='\n')	linenum++;
				else if (c==' ' || c=='\t')	continue;
				else break;
			}
		}
	} //end while
	if (isalpha(c))			return (alpha_process());	
	else if (isdigit(c))	return (digit_process());	
	else										
	{
		switch(c)
		{
		case '+':
			if (fin.peek()=='+' && fin.get(c))
				return (token(get_int("++"),"++"));
			else if (fin.peek()=='=' && fin.get(c))
				return (token(get_int("+="),"+="));
			else if ((prev_c=='(' || prev_c=='=') && isdigit(fin.peek()))
				return (digit_process());
			else
				return (token(get_int("+"), "+"));
		case '-':
			if (fin.peek()=='-' && fin.get(c))
				return (token(get_int("--"),"--"));
			else if (fin.peek()=='=' && fin.get(c))
				return (token(get_int("-="),"-="));
			else if ((prev_c=='(' || prev_c=='=') && isdigit(fin.peek()))
				return (digit_process());
			else
				return (token(get_int("-"), "-"));
		case '*':
			if (fin.peek()=='=' && fin.get(c))
				return (token(get_int("*="), "*="));
			else
				return (token(get_int("*"),"*"));
		case '/':
			if (fin.peek()=='=' && fin.get(c))
				return (token(get_int("/="),"/="));
			else
				return (token(get_int("/"),"/"));
		case '=':
			if (fin.peek()=='=' && fin.get(c))
				return (token(get_int("=="), "=="));
			else
				return (token(get_int("="),"="));
		case '<':

			if (fin.peek()=='=' && fin.get(c))
				return (token(get_int("<="),"<="));
			else if (fin.peek()=='<' && fin.get(c))
				return (token(get_int("<<"),"<<"));
			else
				return (token(get_int("<"),"<"));
		case '>':
			if (fin.peek()=='=' && fin.get(c))
				return (token(get_int(">="),">="));
			else if (fin.peek()=='>' && fin.get(c))
				return (token(get_int(">>"),">>"));
			else
				return (token(get_int(">"),">"));
		case '!':
			if (fin.peek()=='=' && fin.get(c))
				return (token(get_int("!="),"!="));
			else
				return (token(get_int("!"),"!"));
		case '&':
			if (fin.peek()=='&' && fin.get(c))
				return (token(get_int("&&"),"&&"));
			else if (fin.peek()=='=' && fin.get(c))
				return (token(get_int("&="),"&="));
			else
				return (token(get_int("&"),"&"));
		case '|':
			if (fin.peek()=='|' && fin.get(c))
				return (token(get_int("||"), "||"));
			else if (fin.peek()=='=' && fin.get(c))
				return (token(get_int("|="),"|="));
			else
				return (token(get_int("|"),"|"));
		case '~':
			if (fin.peek()=='=' && fin.get(c))
				return (token(get_int("~="), "~="));
			else
				return (token(get_int("~"),"~"));
		case '(':		return (token(get_int("("), "("));
		case ')':		return (token(get_int(")"), ")"));
		case '[':		return (token(get_int("["), "["));
		case ']':		return (token(get_int("]"), "]"));
		case '{':		return (token(get_int("{"), "{"));
		case '}':		return (token(get_int("}"), "}"));
		case '"':		return (string_process());
		case ';':		return (token(get_int(";"),";"));
		case ',':		return (token(get_int(","),","));
		case '.':		return (token(get_int("."),"."));
		case ':':		return (token(get_int(":"),":"));
		case '\'':
			i = 0;
			line[i++] = c;
			fin.get(c);
			line[i++] = c;
			fin.get(c);
			line[i++] = c;
			if(c!='\'')
			{
				fin.get(c);
				line[i++] = c;
			}
			else if(c=='\'' && fin.peek()=='\'')
			{
				fin.get(c);
				line[i++] = c;
			}
			line[i] = '\0';
			return (token(get_int("\'"),s.assign(line)));
		case '#':
			while ((c==' ' || c=='\t') && fin.get(c));
			i = 0;
			line[i++] = c;
			while (fin.get(c) && isalpha(c))
				line[i++] = c;
			s.assign(line);
			if (s.compare("#include")==0)
				return (token(get_int("#include"),"#include"));
			else if (s.compare("#define")==0)
				return (token(get_int("#define"),"#define"));
			else
			{
				std::cerr << "line " << linenum
					<< ", after # must be include or define.\n";
				return (token(s));
			}
		default:
			std::cout << "Error, line " << linenum
				<< ":\tillegal character \'" << c << "\'\n";
			line[0] = c;
			line[1] = '\0';
			return (token(get_int("ILLEGAL"), s.assign(line)));
		}
	}
	return token();
}

void scanner::writetofile(const token& tok)
{
	if (tok.tag==0)
		fout << tok.val << '\n';
	else if (tok.tag > 0)
		fout << tok.tag << ',' << tok.val << '\n';
}

void scanner::print_token(const token& tok)
{
	std::cout << tok.tag << " " << tok.val << std::endl;
}
