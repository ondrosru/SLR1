#pragma once
#include <string>
#include <vector>
#include <set>

struct GrammarSymbol {
	std::string value;
	bool isTerminal;

	bool operator<( const GrammarSymbol& symbol ) const {
		return value < symbol.value;
	}

	bool operator==( const GrammarSymbol& symbol ) const {
		return value == symbol.value && isTerminal == symbol.isTerminal;
	}
};

struct RelationFirst {
	GrammarSymbol value;
	size_t ruleNum;

	bool operator<( const RelationFirst& relationFirst ) const {
		return value < relationFirst.value
			|| (value == relationFirst.value && ruleNum < relationFirst.ruleNum);
	}
	bool operator==( const RelationFirst& relationFirst ) const {
		return value == relationFirst.value && ruleNum == relationFirst.ruleNum;
	}
};

struct Rule {
	GrammarSymbol left;
	std::vector<GrammarSymbol> right;
	std::set<RelationFirst> guideSet;
};

struct Position {
	size_t rule;
	size_t pos;

	bool operator<( const Position& position ) const {
		return rule < position.rule || (rule == position.rule && pos < position.pos);
	}
};

struct Transition {
	GrammarSymbol symbol;
	int count;
	std::set<Position> positions;
	bool isTransition;
};