#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <queue>
#include "GrammarType.h"

const GrammarSymbol EMPTY_SYMBOL = { "@", false };
const GrammarSymbol END_SYMBOL = { "[end]", true };
const char DELIM = '~';
const char COMMA = ',';
const std::string OK_STRING = "|ok|";
const std::string CONVOLUTION_STR = "|R|";
const std::string NO_VALUE = "|-|";
const std::string UNDERLINE_STR = "_";

GrammarSymbol ToGrammarSymbol( std::string value ) {
	GrammarSymbol symbol = { value, true };
	if ( value.size() > 2 && value[0] == '<' && value[value.size() - 1] == '>' ) {
		symbol.isTerminal = false;
	}
	if (value == EMPTY_SYMBOL.value) {
		symbol.isTerminal = false;
	}
	return symbol;
}

std::vector<Rule> ReadRules( std::string fileName ) {
	std::vector<Rule> rules;
	std::string line;
	std::ifstream fileStream( fileName );
	while ( std::getline( fileStream, line ) ) {
		std::stringstream lineStream( line );
		std::string symbol;
		Rule rule;
		lineStream >> symbol;
		rule.left = ToGrammarSymbol( symbol );
		lineStream >> symbol;
		while ( lineStream.rdbuf()->in_avail() != 0 ) {
			lineStream >> symbol;
			rule.right.push_back( ToGrammarSymbol( symbol ) );
		}
		rules.push_back( rule );
	}
	return rules;
}

std::set<RelationFirst> First( GrammarSymbol symbol, std::vector<Rule> rules ) {
	std::set<RelationFirst> relationsFirst;
	for ( size_t i = 0; i < rules.size(); i++ ) {
		if ( rules[i].left == symbol ) {
			relationsFirst.insert( { rules[i].right[0], i } );
			if ( !rules[i].right[0].isTerminal ) {
				if ( symbol != rules[i].right[0] ) {
					std::set<RelationFirst> rfs = First( rules[i].right[0], rules );
					relationsFirst.insert( rfs.begin(), rfs.end() );
				}
			}
		}
	}
	return relationsFirst;
}

std::set<GrammarSymbol> Follow( GrammarSymbol symbol, std::vector<Rule> rules ) {
	std::set<GrammarSymbol> follows;
	for ( Rule rule : rules ) {
		for ( size_t i = 0; i < rule.right.size(); i++ ) {
			if ( rule.right[i] == symbol ) {
				if ( rule.right.size() > i + 1 ) {
					follows.insert( rule.right[i + 1] );
				}
				else {
					if ( symbol != rule.left ) {
						std::set<GrammarSymbol> newFollows = Follow( rule.left, rules );
						follows.insert( newFollows.begin(), newFollows.end() );
					}
					
				}
			}
		}
	}
	return follows;
}

void CreateTableRecursion( TableType& table, std::vector<Rule> rules, std::pair<GrammarSymbol, std::set<Position>> state ) {
	std::set<std::string> currRow;
	for ( Position pos : state.second ) {
		std::string str = state.first.value;
		str += std::to_string( pos.rule );
		str += DELIM;
		str += std::to_string( pos.pos );
		currRow.insert( str );
	}

	if ( table.find( currRow ) == table.end() ) {
		std::map<GrammarSymbol, std::set<Position>> states;
		for ( Position pos : state.second ) {
			if ( rules[pos.rule].right.size() > pos.pos + 1 ) {
				GrammarSymbol currSymbol = rules[pos.rule].right[pos.pos + 1];
				if ( currSymbol != END_SYMBOL ) {
					states[currSymbol].insert( { pos.rule, pos.pos + 1 } );
					std::string str = currSymbol.value;
					str += std::to_string( pos.rule );
					str += DELIM;
					str += std::to_string( pos.pos + 1 );
					table[currRow][currSymbol.value].values.insert( str );
				}
				else {
					std::string str = CONVOLUTION_STR;
					str += std::to_string( pos.rule );
					table[currRow][currSymbol.value].values.insert( str );
					table[currRow][currSymbol.value].count = rules[pos.rule].right.size();
				}
				
				if ( !currSymbol.isTerminal ) {
					std::set<RelationFirst> relationsFirst = First( currSymbol, rules );
					for ( RelationFirst rf : relationsFirst ) {
						if ( rf.value == EMPTY_SYMBOL ) {
							std::set<GrammarSymbol> follows = Follow( rules[rf.ruleNum].left, rules );
							for ( GrammarSymbol follow : follows ) {
								std::string str = CONVOLUTION_STR;
								str += std::to_string( pos.rule );
								table[currRow][follow.value].values.insert( str );
								table[currRow][follow.value].count = 0;
							}
						}
						else {
							states[rf.value].insert( { rf.ruleNum, 0 } );
							std::string str = rf.value.value;
							str += std::to_string( rf.ruleNum );
							str += DELIM;
							str += '0';
							table[currRow][rf.value.value].values.insert( str );
							table[currRow][rf.value.value].count = -1;
						}
					}
				}
			}
			else {
				std::set<GrammarSymbol> follows = Follow( rules[pos.rule].left, rules );
				for ( GrammarSymbol follow : follows ) {
					std::string str = CONVOLUTION_STR;
					str += std::to_string( pos.rule );
					table[currRow][follow.value].values.insert( str );
					table[currRow][follow.value].count = rules[pos.rule].right.size();
				}
			}
		}

		for ( std::pair<GrammarSymbol, std::set<Position>> state : states ) {
			CreateTableRecursion( table, rules, state );
		}
	}
}

TableType CreateTable( std::vector<Rule> rules ) {
	TableType table;
	std::set<std::string> currRow;
	currRow.insert( rules[0].left.value );
	table[currRow][rules[0].left.value].values.insert( OK_STRING );
	std::set<RelationFirst> relationsFirst = First( rules[0].left, rules );
	std::map<GrammarSymbol, std::set<Position>> states;
	for ( RelationFirst rf : relationsFirst ) {
		if ( rf.value == EMPTY_SYMBOL ) {
			std::set<GrammarSymbol> follows = Follow( rules[rf.ruleNum].left, rules );
			for ( GrammarSymbol follow : follows ) {
				std::string str = CONVOLUTION_STR;
				str += std::to_string( rf.ruleNum );
				table[currRow][follow.value].values.insert( str );
				table[currRow][follow.value].count = 0;
			}
		}
		else {
			states[rf.value].insert( { rf.ruleNum, 0 } );
			std::string str = rf.value.value;
			str += std::to_string( rf.ruleNum );
			str += DELIM;
			str += '0';
			table[currRow][rf.value.value].values.insert( str );
			table[currRow][rf.value.value].count = -1;
		}
	}
	for ( std::pair<GrammarSymbol, std::set<Position>> state : states ) {
		CreateTableRecursion( table, rules, state );
	}
	return table;
}

bool isNonterminal( std::string str ) {
	return str.size() > 2 && str[0] == '<' && str[str.size() - 1] == '>';
}

void PrintTable( std::vector<Rule> rules, TableType table, std::ostream& stream ) {
	GrammarSymbol startSymbol = rules[0].left;

	std::vector<std::string> terminals;
	std::vector<std::string> nonTerminals;
	std::vector<std::string> cols;
	for ( auto row : table ) {
		for ( auto col : row.second ) {
			if ( col.first != END_SYMBOL.value ) {
				if ( col.first == rules[0].left.value ) {
					if ( std::find( cols.begin(), cols.end(), col.first ) == cols.end() ) {
						cols.push_back( col.first );
					}
				}
				else if ( std::find( terminals.begin(), terminals.end(), col.first ) == terminals.end() && std::find( nonTerminals.begin(), nonTerminals.end(), col.first ) == nonTerminals.end() ) {
					if ( isNonterminal( col.first ) ) {
						nonTerminals.push_back( col.first );
					}
					else {
						terminals.push_back( col.first );
					}
				}
			}
		}
	}

	cols.insert( cols.end(), nonTerminals.begin(), nonTerminals.end() );
	cols.insert( cols.end(), terminals.begin(), terminals.end() );
	cols.push_back( END_SYMBOL.value );

	for ( size_t i = 0; i < cols.size(); i++ ) {
		stream << cols[i];
		if ( i + 1 < cols.size() ) {
			stream << " ";
		}
	}
	stream << std::endl;

	std::set<std::string> startRow;
	startRow.insert( startSymbol.value );
	std::queue<std::set<std::string>> queue;
	queue.push( startRow );

	std::vector<std::set<std::string>> positions;
	positions.push_back( startRow );

	while ( !queue.empty() ) {
		std::set<std::string> currRow = queue.front();
		queue.pop();
		size_t pos = 0;
		for ( size_t i = 0; i < positions.size(); i++ ) {
			if ( positions[i] == currRow ) {
				pos = i + 1;
			}
		}
		stream << pos << " ";

		size_t n = 0;
		for ( auto col : cols) {
			if ( table[currRow].find( col ) != table[currRow].end() ) {
				if ( table[currRow][col].values.size() == 1 ) {
					for ( std::string value : table[currRow][col].values ) {
						if ( value == OK_STRING ) {
							stream << OK_STRING;
						}
						else if ( value.size() > CONVOLUTION_STR.size() && value.substr( 0, CONVOLUTION_STR.size() ) == CONVOLUTION_STR ) {
							std::string numStr = value.substr( CONVOLUTION_STR.size() );
							int num = atoi( numStr.c_str() );
							stream << CONVOLUTION_STR << rules[num].left.value << DELIM << table[currRow][col].count;
						}
						else {
							size_t num = 0;
							if ( std::find( positions.begin(), positions.end(), table[currRow][col].values ) == positions.end() ) {
								positions.push_back( table[currRow][col].values );
								num = positions.size();
								queue.push( table[currRow][col].values );
							}
							else {
								for ( size_t i = 0; i < positions.size(); i++ ) {
									if ( positions[i] == table[currRow][col].values ) {
										num = i + 1;
									}
								}
							}
							stream << num;
						}
					}
				}
				else {
					size_t num = 0;
					if ( std::find( positions.begin(), positions.end(), table[currRow][col].values ) == positions.end() ) {
						positions.push_back( table[currRow][col].values );
						num = positions.size();
						queue.push( table[currRow][col].values );
					}
					else {
						for ( size_t i = 0; i < positions.size(); i++ ) {
							if ( positions[i] == table[currRow][col].values ) {
								num = i + 1;
							}
						}
					}
					stream << num;
				}
			}
			else {
				stream << NO_VALUE;
			}
			n++;
			if ( n < cols.size() ) {
				stream << " ";
			}
		}

		stream << std::endl;
	}
}
