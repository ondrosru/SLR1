#include "GrammarHeader.h"
#include <map>

using Table = std::vector<std::map<GrammarSymbol, std::string>>;

class GeneratorSLR {
public:
	GeneratorSLR() {};
	~GeneratorSLR() {};
	Table generateSLR( std::vector<Rule> rules ) {
		Table table;
		std::vector<std::set<Transition>> rows;
		std::map<GrammarSymbol, std::string> row;
		row[rules[0].left] = "ok";
		std::set<Transition> relationsFirst = getFirst( 0, rules );
		std::map<GrammarSymbol, std::set<Transition>> states;
		for ( Transition rf : relationsFirst ) {
			if ( rf.symbol.value == EMPTY_SYMBOL.value ) {
				std::set<Transition> follows = getFollow( rf.ruleNum, rules );
				for ( Transition follow : follows ) {
					row[follow.symbol] = "R" + rules[rf.ruleNum].left.value + "0";
				}
			}
			else {
				states[rf.symbol].insert( rf );
			}

		}
		for ( auto state : states ) {
			rows.push_back( state.second );
			row[state.second.begin()->symbol] = std::to_string( rows.size() + 1 );
		}
		table.push_back( row );

		for ( size_t i = 0; i < rows.size(); i++ ) {
			row = std::map<GrammarSymbol, std::string>();
			states = std::map<GrammarSymbol, std::set<Transition>>();

			for ( Transition transition : rows[i] ) {
				if ( rules[transition.ruleNum].right.size() > transition.pos + 1 ) {
					GrammarSymbol currSymbol = rules[transition.ruleNum].right[transition.pos + 1];
					if ( currSymbol.value == END_SYMBOL.value ) {
						row[END_SYMBOL] = "R" + rules[transition.ruleNum].left.value + std::to_string( rules[transition.ruleNum].right.size() );
					}
					else if ( currSymbol.value == EMPTY_SYMBOL.value ) {
						std::set<Transition> follows = getFollow( transition.ruleNum, rules );
						for ( Transition follow : follows ) {
							row[follow.symbol] = "R" + rules[transition.ruleNum].left.value + "0";
						}
					}
					else {
						states[currSymbol].insert( { currSymbol, transition.ruleNum, transition.pos + 1 } );
					}
					if ( !currSymbol.isTerminal ) {
						std::set<Transition> firsts;
						for ( size_t k = 0; k < rules.size(); k++ ) {
							if ( currSymbol.value == rules[k].left.value ) {
								std::set<Transition> rfs = getFirst( k, rules );
								firsts.insert( rfs.begin(), rfs.end() );
							}
						}

						for ( Transition first : firsts ) {
							if ( first.symbol.value == EMPTY_SYMBOL.value ) {
								std::set<Transition> follows = getFollow( first.ruleNum, rules );
								for ( Transition follow : follows ) {
									row[follow.symbol] = "R" + rules[first.ruleNum].left.value + "0";
								}
							}
							else {
								states[first.symbol].insert( first );
							}
						}
					}

				}
				else {
					std::set<Transition> follows = getFollow( transition.ruleNum, rules );
					for ( Transition follow : follows ) {
						row[follow.symbol] = "R" + rules[transition.ruleNum].left.value + std::to_string( rules[transition.ruleNum].right.size() );
					}
				}
			}

			for ( auto state : states ) {
				if ( std::find( rows.begin(), rows.end(), state.second ) == rows.end() ) {
					rows.push_back( state.second );
					row[state.second.begin()->symbol] = std::to_string( rows.size() + 1 );
				}
				else {
					size_t foundIndex = 0;
					for ( size_t j = 0; j < rows.size(); j++ ) {
						std::set<Transition> rowTransition = rows[j];
						if ( rowTransition == state.second ) {
							foundIndex = j + 2;
						}
					}
					row[state.second.begin()->symbol] = std::to_string( foundIndex );
				}
			}
			table.push_back( row );
		}
		return table;
	};
private:
	struct Transition {
		GrammarSymbol symbol;
		size_t ruleNum;
		size_t pos;

		bool operator<( const Transition& transition ) const {
			return symbol.value < transition.symbol.value
				|| (symbol.value == transition.symbol.value && ruleNum < transition.ruleNum)
				|| (symbol.value == transition.symbol.value && ruleNum == transition.ruleNum && pos < transition.pos);
		};

		bool operator==( const Transition& transition ) const {
			return symbol.value == transition.symbol.value && ruleNum == transition.ruleNum && pos == transition.pos;
		};
	};

	std::set<Transition> getFirst( size_t ruleNum, std::vector<Rule> rules ) {
		std::set<Transition> guides;
		guides.insert( { rules[ruleNum].right[0], ruleNum, 0 } );
		if ( !rules[ruleNum].right[0].isTerminal ) {
			if ( rules[ruleNum].right[0].value != EMPTY_SYMBOL.value ) {
				std::vector<size_t> rulesNum;
				for ( size_t i = 0; i < rules.size(); i++ ) {
					if ( rules[i].left.value == rules[ruleNum].right[0].value && rules[i].left.value != rules[ruleNum].left.value ) {
						rulesNum.push_back( i );
					}
				}
				for ( size_t nextRuleNum : rulesNum ) {
					std::set<Transition> newGuides = getFirst( nextRuleNum, rules );
					guides.insert( newGuides.begin(), newGuides.end() );
				}
			}
		}
		return guides;
	}

	std::set<Transition> getFirstForFollow( size_t ruleNum, std::vector<Rule> rules ) {
		std::set<Transition> guides;
		if ( rules[ruleNum].right[0].isTerminal ) {
			guides.insert( { rules[ruleNum].right[0], ruleNum, 0 } );
		}
		else {
			if ( rules[ruleNum].right[0].value == EMPTY_SYMBOL.value ) {
				std::set<Transition> follows = getFollow( ruleNum, rules );
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
					std::set<Transition> newGuides = getFirstForFollow( nextRuleNum, rules );
					guides.insert( newGuides.begin(), newGuides.end() );
				}
			}
		}
		return guides;
	}

	std::set<Transition> getFollow( size_t ruleNum, std::vector<Rule> rules ) {
		std::set<Transition> follows;
		for ( size_t i = 0; i < rules.size(); i++ ) {
			for ( size_t j = 0; j < rules[i].right.size(); j++ ) {
				if ( rules[ruleNum].left.value == rules[i].right[j].value ) {
					if ( rules[i].right.size() > j + 1 ) {
						if ( rules[i].right[j + 1].isTerminal ) {
							follows.insert( { rules[i].right[j + 1], i, j + 1 } );
						}
						else {
							if ( rules[i].right[j + 1].value == EMPTY_SYMBOL.value ) {
								if ( rules[ruleNum].left.value != rules[i].left.value ) {
									std::set<Transition> newFollows = getFollow( i, rules );
									follows.insert( newFollows.begin(), newFollows.end() );
								}
							}
							else {
								std::vector<size_t> rulesNum;
								for ( size_t k = 0; k < rules.size(); k++ ) {
									if ( rules[k].left.value == rules[i].right[j + 1].value ) {
										rulesNum.push_back( k );
									}
								}
								for ( size_t nextRuleNum : rulesNum ) {
									std::set<Transition> newFirsts = getFirstForFollow( nextRuleNum, rules );
									follows.insert( newFirsts.begin(), newFirsts.end() );
								}
							}
						}
					}
					else {
						if ( rules[ruleNum].left.value != rules[i].left.value ) {
							std::set<Transition> newFollows = getFollow( i, rules );
							follows.insert( newFollows.begin(), newFollows.end() );
						}
					}
				}
			}
		}
		return follows;
	}
};