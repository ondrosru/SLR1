#pragma once
#include <vector>
#include <string>
#include <set>

struct GrammarSymbol {
	std::string value;
	bool isTerminal = false;

	bool operator<( const GrammarSymbol& symbol ) const {
		if ( symbol.isTerminal && isTerminal ) {
			return value < symbol.value;
		}
		else if ( !symbol.isTerminal && !isTerminal ) {
			return value < symbol.value;
		}
		else if ( symbol.isTerminal ) {
			return true;
		}
		return false;
	}

	bool operator==( const GrammarSymbol& symbol ) const {
		return symbol.value == value;
	}
};
struct Rule {
	GrammarSymbol left;
	std::vector<GrammarSymbol> right;
	std::set<GrammarSymbol> guides = std::set<GrammarSymbol>();
};

const GrammarSymbol END_SYMBOL = { "#", true };
const GrammarSymbol EMPTY_SYMBOL = { "@", false };
