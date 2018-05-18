#include <cstdlib>
#include <iomanip>
#include <iostream>
#include "SetLimits.h"
//#include "CodeGenerator.h"
#include "SyntacticalAnalyzer.h"

using namespace std;

int main (int argc, char * argv[])
{
	if (argc < 2)
	{
		printf ("format: %s <filename>\n", argv[0]);
		exit (1);
	}
	SetLimits ();
	SyntacticalAnalyzer parse (argv[1]);
    //if (parse.GetErrors() == 0) {
        //string filename = argv[1];
        //CodeGen cppGen (filename, parse.GetLex());
    //}
	return 0;
}
