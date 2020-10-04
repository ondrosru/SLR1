#include "Lexer.h"
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <stack>
#include <set>

class SyntaxCheck {
public:
	SyntaxCheck( std::string sourcePath, std::string tablePath )
		:m_lexer( sourcePath )
	{
		ReadTable( tablePath );
	};
	~SyntaxCheck() {};

	std::string Check() {
		std::string massage = "";
		m_currRow = m_firstRow;
		m_stack.push( m_firstRow );

		m_currentToken = m_lexer.GetToken();
		while ( massage == "" ) {
			while ( m_currentToken.type == TokenType::EndLine ) {
				m_currentToken = m_lexer.GetToken();
			}

			std::string colName = convertToString( m_currentToken.type );
			if ( m_table[m_currRow].find( colName ) != m_table[m_currRow].end()
				|| m_table[m_currRow].find( m_currentToken.value ) != m_table[m_currRow].end() ) {
				SyntaxTableState state;
				if ( m_table[m_currRow].find( colName ) != m_table[m_currRow].end() ) {
					state = m_table[m_currRow][colName];
				}
				else {
					state = m_table[m_currRow][m_currentToken.value];
				}
				if ( state.type == TypeTransition::Transition ) {
					m_stack.push( m_currRow );
					m_currRow = state.value;
					m_currentToken = m_lexer.GetToken();
				}
				else {
					for ( int i = 0; i < state.count; i++ ) {
						m_currRow = m_stack.top();
						m_stack.pop();
					}
					state = m_table[m_currRow][state.value];
					if ( state.value == OK_STRING ) {
						massage = "ok";
					}
					else {
						m_stack.push( m_currRow );
						m_currRow = state.value;
					}
				}
			}
			else {
				massage = "Error: row-";
				massage += std::to_string( m_currentToken.row );
				massage += " col-";
				massage += std::to_string( m_currentToken.col );
				massage += "\nÂñòðåòèëîñü: ";
				if ( m_currentToken.type == TokenType::End ) {
					massage += convertToString( m_currentToken.type );
				}
				else {
					massage += m_currentToken.value;
				}
				massage += "\nÎæèäàëîñü: ";
				std::set<std::string> values;
				for ( auto row : m_table[m_currRow] ) {
					values.insert( row.first );
				}

				bool delimF = false;
				for ( std::string value : values ) {
					if ( !isNonterm( value ) ) {
						if ( delimF ) {
							massage += ", ";
						}
						massage += value;
						delimF = true;
					}
				}
				massage += "\n";
				
			}
		}
		
		while ( !m_stack.empty() ) {
			m_stack.pop();
		}
		return massage;
	};
	

private:
	const std::string NO_VALUE = "-";
	const std::string CONVOLUTION_STR = "R";
	const std::string OK_STRING = "ok";
	const std::string END_SYMBOL = "#";
	Lexer m_lexer;
	std::stack<std::string> m_stack;
	std::string m_currRow;
	Token m_currentToken;

	std::string m_firstRow;
	std::string m_firstCol;
	enum class TypeTransition {
		Transition,
		Ñonvolution,
	};
	struct SyntaxTableState {
		std::string value;
		TypeTransition type;
		int count;
	};
	std::map<std::string, std::map<std::string, SyntaxTableState>> m_table;

	void ReadTable( std::string fileName ) {
		m_firstRow = "";
		m_firstCol = "";
		std::ifstream file( fileName );
		std::string colsStr;
		std::getline( file, colsStr );
		std::stringstream colsStream( colsStr );
		std::vector<std::string> cols;
		std::string col;
		colsStream >> col;
		while ( colsStream.rdbuf()->in_avail() != 0 ) {
			colsStream >> col;
			if ( col != "" ) {
				if ( m_firstCol == "" ) {
					m_firstCol = col;
				}
				cols.push_back( col );
			}
		}

		std::string line;
		while ( std::getline( file, line ) ) {
			std::string rowNum;
			std::stringstream lineStream( line );
			lineStream >> rowNum;

			if ( m_firstRow == "" ) {
				m_firstRow = rowNum;
			}

			for ( size_t i = 0; i < cols.size(); i++ ) {
				std::string value;
				lineStream >> value;
				if ( value != NO_VALUE ) {
					if ( value.size() > CONVOLUTION_STR.size() && value.substr( 0, CONVOLUTION_STR.size() + 1 ) == CONVOLUTION_STR + "<" ) {
						std::string nextState = value.substr( CONVOLUTION_STR.size() );
						std::stringstream streamValue( nextState );
						std::string nextSymbol = "";
						std::string count;
						std::getline( streamValue, nextSymbol, '>' );
						nextSymbol += '>';
						std::getline( streamValue, count );
						SyntaxTableState valueTable = { nextSymbol, TypeTransition::Ñonvolution, std::atoi(count.c_str()) };
						m_table[rowNum][cols[i]] = valueTable;
					}
					else {
						SyntaxTableState valueTable = { value, TypeTransition::Transition, 0 };
						m_table[rowNum][cols[i]] = valueTable;
					}
				}
			}
		}
	};

	bool isNonterm( std::string str ) {
		return str.size() > 2 && str[0] == '<' && str[str.size() - 1] == '>';
	}

	std::string convertToString( TokenType type ) {
		std::string str;
		switch ( type ) {
		case TokenType::Identifier:
			str = "[identifier]";
			break;
		case TokenType::Integer:
			str = "[integer]";
			break;
		case TokenType::Float:
			str = "[float]";
			break;
		case TokenType::HexNumber:
			str = "[hexnumber]";
			break;
		case TokenType::BigFloat:
			str = "[bigfloat]";
			break;
		case TokenType::BinaryNumber:
			str = "[binarynumber]";
			break;
		case TokenType::Char:
			str = "[char]";
			break;
		case TokenType::String:
			str = "[string]";
			break;
		case TokenType::Keyword:
			str = "[keyword]";
			break;
		case TokenType::LeftRoundBracket:
			str = "[leftroundbracket]";
			break;
		case TokenType::RightRoundBracket:
			str = "[rightroundbracket]";
			break;
		case TokenType::LeftSquareBracket:
			str = "[leftsquarebracket]";
			break;
		case TokenType::RightSquareBracket:
			str = "[rightsquarebracket]";
			break;
		case TokenType::LeftCurlyBracket:
			str = "[leftcurlybracket]";
			break;
		case TokenType::RightCurlyBracket:
			str = "[rightcurlybracket]";
			break;
		case TokenType::LessThan:
			str = "[lessthan]";
			break;
		case TokenType::GreaterThan:
			str = "[greaterthan]";
			break;
		case TokenType::Equal:
			str = "[equal]";
			break;
		case TokenType::Assignment:
			str = "[assignment]";
			break;
		case TokenType::Plus:
			str = "[plus]";
			break;
		case TokenType::Minus:
			str = "[minus]";
			break;
		case TokenType::Asterisk:
			str = "[asterisk]";
			break;
		case TokenType::Slash:
			str = "[slash]";
			break;
		case TokenType::Backslash:
			str = "[backslash]";
			break;
		case TokenType::Dot:
			str = "[dot]";
			break;
		case TokenType::Comma:
			str = "[comma]";
			break;
		case TokenType::Colon:
			str = "[colon]";
			break;
		case TokenType::Semicolon:
			str = "[semicolon]";
			break;
		case TokenType::End:
			str = "#";
			break;
		case TokenType::EndLine:
			str = "[endline]";
			break;
		case TokenType::Unexpected:
			str = "[unexpected]";
			break;
		default:
			throw "Error: invalid token type";
			break;
		}
		return str;
	}
};