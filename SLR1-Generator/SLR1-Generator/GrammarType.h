#pragma once
#include <string>
#include <vector>
#include <set>

struct TableValue {
	std::set<std::string> values;
	size_t count;
};
using TableType = std::map<std::set<std::string>, std::map<std::string, TableValue>>;

struct GrammarSymbol {
	std::string value;
	bool isTerminal;

	bool operator<( const GrammarSymbol& symbol ) const {
		return value < symbol.value;
	}

	bool operator==( const GrammarSymbol& symbol ) const {
		return value == symbol.value;
	}

	bool operator!=( const GrammarSymbol& symbol ) const {
		return value != symbol.value;
	}
};

struct RelationFirst {
	GrammarSymbol value;
	size_t ruleNum;

	bool operator<( const RelationFirst& relationFirst ) const {
		return value < relationFirst.value
			|| (value == relationFirst.value && ruleNum < relationFirst.ruleNum);
	}
};

struct Rule {
	GrammarSymbol left;
	std::vector<GrammarSymbol> right;
};

struct Position {
	size_t rule;
	size_t pos;

	bool operator<( const Position& position ) const {
		return rule < position.rule || (rule == position.rule && pos < position.pos);
	}
};
