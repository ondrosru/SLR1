#include "GrammarReader.hpp"
#include "GeneratorSLR.hpp"
#include <iostream>
#include <iomanip>

void printTable( std::ostream& stream, Table table);

int main( int argc, char* argv[] ) {
	GrammarReader reader;
	std::vector<Rule> rules;
	try {
		rules = reader.readGrammar( "in2.txt" );
	}
	catch ( const std::exception& e ) {
		std::cerr << e.what() << std::endl;
		return -1;
	}
	GeneratorSLR generatorSLR;
	Table table = generatorSLR.generateSLR( rules );
	std::ofstream file( "out.txt" );
	printTable( file, table );
	return 0;
}

void printTable( std::ostream& stream, Table table ) {
	std::map<GrammarSymbol, size_t> maxLengthCols;
	std::vector<GrammarSymbol> nonterminals;
	std::vector<GrammarSymbol> terminals;
	for ( size_t i = 0; i < table.size(); i++ ) {
		for ( std::pair<GrammarSymbol, std::string> cell : table[i] ) {
			if ( maxLengthCols.find( cell.first ) == maxLengthCols.end() ) {
				maxLengthCols[cell.first] = cell.first.value.size();
				if ( maxLengthCols[cell.first] < cell.second.size() ) {
					maxLengthCols[cell.first] = cell.second.size();
				}
			}
			else {
				if ( maxLengthCols[cell.first] > cell.first.value.size() ) {
					maxLengthCols[cell.first] = cell.first.value.size();
				}
				if ( maxLengthCols[cell.first] < cell.second.size() ) {
					maxLengthCols[cell.first] = cell.second.size();
				}
			}
			if ( cell.first.isTerminal ) {
				if ( std::find( terminals.begin(), terminals.end(), cell.first ) == terminals.end() ) {
					terminals.push_back( cell.first );
				}
			}
			else {
				
				if ( std::find( nonterminals.begin(), nonterminals.end(), cell.first ) == nonterminals.end() ) {
					nonterminals.push_back( cell.first );
				}
			}
		} 
	}

	for ( std::map<GrammarSymbol, size_t>::iterator it = maxLengthCols.begin(); it != maxLengthCols.end(); it++ ) {
		it->second += 1;
	}

	std::vector<GrammarSymbol> nameCols;
	nameCols.insert( nameCols.end(), nonterminals.begin(), nonterminals.end() );
	nameCols.insert( nameCols.end(), terminals.begin(), terminals.end() );

	stream << std::setw(4) << "#";
	for ( GrammarSymbol nameCol : nameCols ) {
		stream << std::setw( maxLengthCols[nameCol] ) << nameCol.value;
	}
	stream << std::endl;

	for ( size_t i = 0; i < table.size(); i++ ) {
		stream << std::setw( 4 ) << i + 1;
		for ( GrammarSymbol nameCol : nameCols ) {
			if ( table[i].find( nameCol ) != table[i].end() ) {
				stream << std::setw( maxLengthCols[nameCol] ) << table[i][nameCol];
			}
			else {
				stream << std::setw( maxLengthCols[nameCol] ) << "-";
			}
		}
		stream << std::endl;
	}
}