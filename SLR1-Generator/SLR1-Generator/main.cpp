#include <iostream>
#include <fstream>
#include "Grammar.hpp"


int main(int argc, char* argv[]) {
	std::vector<Rule> rules = ReadRules( "in.txt" );
	TableType table = CreateTable( rules );
	std::ofstream out( "out.txt" );
	PrintTable( rules, table, out );
}