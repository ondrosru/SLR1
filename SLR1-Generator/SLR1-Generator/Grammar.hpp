#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <queue>
#include "GrammarType.h"

const std::string EMPTY_SYMBOL = "@";

GrammarSymbol ToGrammarSymbol( std::string value ) {
	GrammarSymbol symbol;
	symbol.value = value;
	if ( value.size() > 2 && value[0] == '<' && value[value.size() - 1] == '>' ) {
		symbol.isTerminal = false;
	}
	else {
		symbol.isTerminal = true;
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

std::set<RelationFirst> GetFirstPlus( GrammarSymbol symbol, std::vector<Rule> rules ) {
	std::set<RelationFirst> first;
	for ( size_t i = 0; i < rules.size(); i++ ) {
		if ( symbol.value == rules[i].left.value ) {
			RelationFirst rf = { rules[i].right[0], i };
			first.insert( rf );
		}
	}
	return first;
}

std::set<RelationFirst> GetFirstStar( GrammarSymbol symbol, std::vector<Rule> rules ) {
	std::set<RelationFirst> first;
	for ( size_t i = 0; i < rules.size(); i++ ) {
		if ( rules[i].left == symbol ) {
			Rule rule = rules[i];
			RelationFirst rf = { rule.right[0], i };
			first.insert( rf );
			if ( !rf.value.isTerminal ) {
				std::set<GrammarSymbol> processedNonterminals;
				std::queue<GrammarSymbol> nontemrminals;
				nontemrminals.push( rule.right[0] );
				while ( !nontemrminals.empty() ) {
					GrammarSymbol nonterminal = nontemrminals.front();
					nontemrminals.pop();
					processedNonterminals.insert( nonterminal );
					std::set<RelationFirst> rfs = GetFirstPlus( nonterminal, rules );
					for ( RelationFirst rf : rfs ) {
						if ( !rf.value.isTerminal ) {
							if ( processedNonterminals.find( rf.value ) == processedNonterminals.end() ) {
								nontemrminals.push( rf.value );
							}
						}
						first.insert( rf );
					}
				}
			}
		}
	}
	return first;
}

void CreateGuideSets( std::vector<Rule>& rules ) {
	for ( std::vector<Rule>::iterator it = rules.begin(); it != rules.end(); it++ ) {
		std::set<RelationFirst> relationsFirst = GetFirstStar( it->left, rules );
		for ( RelationFirst rf : relationsFirst ) {
			it->guideSet.insert( rf );
		}
	}
}

void CreateTable( std::vector<Rule> rules, std::map<GrammarSymbol, Transition> transitions, std::map<std::set<Position>, std::map<GrammarSymbol, Transition>>& table ) {
	for ( std::pair<GrammarSymbol, Transition> transition : transitions ) {
		if ( transition.second.isTransition && table.find( transition.second.positions ) == table.end() ) {
			std::map<GrammarSymbol, Transition> newTransitions;
			for ( Position position : transition.second.positions ) {
				Rule rule = rules[position.rule];
				if ( rule.right.size() - 1 > position.pos ) {
					Position newPosition = { position.rule, position.pos + 1 };
					
					if ( newTransitions.find( rule.right[newPosition.pos]) != newTransitions.end() ) {
						newTransitions[rule.right[newPosition.pos]].positions.insert( newPosition );
					}
					else {
						Transition newTransition;
						newTransition.isTransition = true;
						newTransition.positions.insert( newPosition );
						newTransitions[rule.right[newPosition.pos]] = newTransition;
					}

					if ( !rule.right[newPosition.pos].isTerminal ) {
						for ( size_t i = 1; i < rules.size(); i++ ) {
							if ( rules[i].left == rule.right[newPosition.pos] ) {
								for ( RelationFirst rf : rules[i].guideSet) {
									if ( rf.value.value == EMPTY_SYMBOL ) {
										Transition newTransition;
										newTransition.isTransition = false;
										newTransition.symbol = rules[rf.ruleNum].left;
										newTransition.count = 0;
										newTransitions[{EMPTY_SYMBOL, true}] = newTransition;
									}
									else {
										Position newPosition2;
										newPosition2.rule = rf.ruleNum;
										newPosition2.pos = 0;

										if ( newTransitions.find( rf.value ) != newTransitions.end() ) {
											newTransitions[rf.value].positions.insert( newPosition2 );
										}
										else {
											Transition newTransition;
											newTransition.isTransition = true;
											newTransition.positions.insert( newPosition2 );
											newTransitions[rf.value] = newTransition;
										}
									}
								}
							}
						}
					}
				}
				else {
					Transition newTransition;
					newTransition.isTransition = false;
					newTransition.symbol = rule.left;
					newTransition.count = rule.right.size();
					newTransitions[{EMPTY_SYMBOL, true}] = newTransition;
				}
				table[transition.second.positions] = newTransitions;
			}

			CreateTable( rules, newTransitions, table );
		}
	}
}
