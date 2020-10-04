#include <sstream>
#include <fstream>
#include "GrammarHeader.h"

class GrammarReader {
public:
	GrammarReader() {};
	~GrammarReader() {};

	std::vector<Rule> readGrammar( std::string fileName ) {
		std::vector<Rule> rules;
		std::ifstream file;
		file.open( fileName );
		if ( file.fail() ) {
			std::string massage = "Error: " + std::string( strerror( errno ) );
			throw std::exception( massage.c_str() );
		}
		rules = readRules( file );
		createGuides( rules );
		return rules;
	};
private:
	std::vector<Rule> readRules( std::ifstream& inputStream ) {
		std::vector<Rule> rules;
		std::string line;
		while ( std::getline( inputStream, line ) ) {
			Rule rule;
			std::stringstream lineStream( line );
			std::string leftSymbol;
			lineStream >> leftSymbol;
			rule.left = convertGrammarSymbol( leftSymbol );
			std::string arrow;
			lineStream >> arrow;
			std::vector<GrammarSymbol> rigthSymbols;
			while ( lineStream.rdbuf()->in_avail() != 0 ) {
				std::string rightSymbol;
				lineStream >> rightSymbol;
				if ( rightSymbol != "" ) {
					rigthSymbols.push_back( convertGrammarSymbol( rightSymbol ) );
				}
			}
			rule.right = rigthSymbols;
			rules.push_back( rule );
		}
		return rules;
	}

	void createGuides( std::vector<Rule>& rules ) {
		for ( size_t i = 0; i < rules.size(); i++ ) {
			rules[i].guides = getFirst( i, rules );
		}
	}

	std::set<GrammarSymbol> getFirst ( size_t ruleNum, std::vector<Rule> rules ) {
		std::set<GrammarSymbol> guides;
		if ( rules[ruleNum].right[0].value != EMPTY_SYMBOL.value ) {
			guides.insert( rules[ruleNum].right[0] );
		}
		if ( !rules[ruleNum].right[0].isTerminal ) {
			if ( rules[ruleNum].right[0].value == EMPTY_SYMBOL.value ) {
				std::set<GrammarSymbol> follows = getFollow( ruleNum, rules );
				guides.insert( follows.begin(), follows.end() );
			}
			else {
				std::vector<size_t> rulesNum;
				for ( size_t i = 0; i < rules.size(); i++ ) {
					if ( rules[i].left.value == rules[ruleNum].right[0].value && rules[i].left.value != rules[ruleNum].left.value ) {
						rulesNum.push_back( i );
					}
				}

				for ( size_t nextRuleNum : rulesNum ) {
					std::set<GrammarSymbol> newGuides = getFirst( nextRuleNum, rules );
					guides.insert( newGuides.begin(), newGuides.end() );
				}
			}
		}
		return guides;
	}

	std::set<GrammarSymbol> getFollow( size_t ruleNum, std::vector<Rule> rules ) {
		std::set<GrammarSymbol> follows;
		for ( size_t i = 0; i < rules.size(); i++ ) {
			for ( size_t j = 0; j < rules[i].right.size(); j++ ) {
				if ( rules[ruleNum].left.value == rules[i].right[j].value ) {
					if ( rules[i].right.size() > j + 1 ) {
						if ( rules[i].right[j + 1].isTerminal ) {
							follows.insert( rules[i].right[j + 1] );
						}
						else {
							if ( rules[i].right[j + 1].value == EMPTY_SYMBOL.value ) {
								std::set<GrammarSymbol> newFollows = getFollow( i, rules );
								follows.insert( newFollows.begin(), newFollows.end() );
							}
							else {
								std::vector<size_t> rulesNum;
								for ( size_t k = 0; k < rules.size(); k++ ) {
									if ( rules[k].left.value == rules[i].right[j + 1].value ) {
										rulesNum.push_back( k );
									}
								}
								for ( size_t nextRuleNum : rulesNum ) {
									std::set<GrammarSymbol> newFirsts = getFirst( nextRuleNum, rules );
									follows.insert( newFirsts.begin(), newFirsts.end() );
								}
							}
						}
					}
					else {
						std::set<GrammarSymbol> newFollows = getFollow( i, rules );
						follows.insert( newFollows.begin(), newFollows.end() );
					}
				}
			}
		}
		return follows;
	}

	GrammarSymbol convertGrammarSymbol( std::string str ) {
		GrammarSymbol symbol;
		if ( str == EMPTY_SYMBOL.value ) {
			symbol = EMPTY_SYMBOL;
		}
		else if ( str == END_SYMBOL.value ) {
			symbol = END_SYMBOL;
		}
		else {
			symbol = { str, !isNonTerminal( str ) };
		}
		return symbol;
	}

	bool isNonTerminal( std::string str ) {
		return str.size() > 2 && str[0] == '<' && str[str.size() - 1] == '>';
	}
};