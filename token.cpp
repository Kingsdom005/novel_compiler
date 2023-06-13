#include <iostream>
#include "token.h"

const string operator_arr[token::OP_SIZE] =
{
	"$",
	"%",
	"&&",
	"(",
	")",
	"*",
	"+",
	"-",
	"/",
	";",
	"<",
	"<=",
	"=",
	"==",
	">",
	">=",
	"break",
	"continue",
	"double",
	"double_num",
	"e",
	"id",
	"if",
	"int",
	"int_num",
	"return",
	"void",
	"while",
	"{",
	"||",
	"}",

	"M", 
	"S0",
	"additive_expression",
	"assignment_expression",
	"compound_statement",
	"decalration_list",
	"declaration",
	"equality_expression",
	"expression",
	"expression_statement",
	"external_declaration",
	"function_definition",
	"function_name",
	"iteration_statement",
	"jump_statement",
	"logical_or_expression",
	"logical_and_expression",
	"multiplicative_expression",
	"parameter",
	"primary_expression",
	"relational_expression",
	"selection_statement",
	"statement",
	"statement_list",
	"translation_unit",
	"type_specifier",
};

token::token(const int& t, const string& v)
{
	tag = t;
	val = v;
}

token::token(const string& v)
{
	tag = 0;
	val = v;
}

token::token()
{
	tag = FILE_END;
	val = string();
}

void token::init_hash()
{
	for (int i=1; i<=OP_SIZE; i++)
	{
		hash_table[operator_arr[i-1]] = i;
	}
}

int token::get_int(const std::string &s)
{
	if (hash_table.size()==0)
		init_hash();

	int x = hash_table[s];
	if (x==0)
		return ERROR;
	return x;
}
