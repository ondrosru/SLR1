#include <iostream>
#include "Grammar.hpp"


int main(int argc, char* argv[]) {
	std::vector<Rule> rules = ReadRules( "in.txt" );
	TableType table = CreateTable( rules );
	PrintTable( rules, table, std::cout );
}