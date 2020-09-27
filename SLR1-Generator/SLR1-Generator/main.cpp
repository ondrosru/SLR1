#include <iostream>
#include "Grammar.hpp"


int main(int argc, char* argv[]) {
	std::vector<Rule> rules = ReadRules( "in.txt" );
	CreateGuideSets( rules );

	std::map<GrammarSymbol, Transition> transitions;
	std::map<std::set<Position>, std::map<GrammarSymbol, Transition>> table;
	for ( RelationFirst rf : rules[0].guideSet ) {
		if ( rf.value.value == EMPTY_SYMBOL ) {
			Transition transition;
			transition.isTransition = false;
			transition.symbol = rules[rf.ruleNum].left;
			transition.count = 0;
			transitions[{EMPTY_SYMBOL, true}] = transition;
		}
		else {
			Position position;
			position.rule = rf.ruleNum;
			position.pos = 0;

			if ( transitions.find( rf.value ) != transitions.end() ) {
				transitions[rf.value].positions.insert( position );
			}
			else {
				Transition transition;
				transition.isTransition = true;
				transition.positions.insert( position );
				transitions[rf.value] = transition;
			}
		}
	}

	CreateTable( rules, transitions, table );
	std::cout << "Hello world!" << std::endl;
}