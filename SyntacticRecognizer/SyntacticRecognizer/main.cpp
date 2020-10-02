#include "SyntaxCheck.hpp"
#include <iostream>

int main( int argc, char* argv[] ) {
	setlocale( LC_ALL, "Russian" );
	SyntaxCheck syntaxCheck( "Source2.txt", "Table2.txt" );
	std::string str = syntaxCheck.Check();
	std::cout << str;
}