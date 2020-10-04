#include "SyntaxCheck.hpp"
#include <iostream>

int main( int argc, char* argv[] ) {
	setlocale( LC_ALL, "Russian" );
	SyntaxCheck syntaxCheck( "Source1.txt", "Table1.txt" );
	std::string str = syntaxCheck.Check();
	std::cout << str;
}