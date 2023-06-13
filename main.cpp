#include <iostream>
#include <string>
#include <list>
#include <iterator>
#include <set>
#include <time.h>
#include "grammar.h"
#include "lr.h"
#include "token.h"
#include "scanner.h"
#include <fstream>
#include <cstdlib>

int main()
{
	using namespace std;


	ifstream fin;
	fin.open("file.txt");
	if (!fin.is_open())
	{
		std::cerr << "file.txt open failed\n";
		return 1;
	}
	string grammarFile, tableFile, analysFile, temp;

	while (fin >> temp)
	{
		if (temp == "grammarFile")
		{
			fin >> grammarFile;

		}
		else if (temp == "tableFile")
		{
			fin >> tableFile;

		}
		else if (temp == "analysFile")
		{
			fin >> analysFile;

		}
	}

	 
	grammar g(grammarFile);
	g.make_items();
	g.write_table(tableFile);
	//g.print_state();
	//g.print_gramma();
	//g.print_table();
	g.write_intermediate_process();
	
	lr mylr(grammarFile,tableFile); 
	if (mylr.analyse(analysFile))
		cout << "\n\nCongratulations, it's received.\n\n";
	else
		cout << "\n\nError, can't received.\n\n";

	//mylr.print_table();

	system("pause");

	return 0;
}
