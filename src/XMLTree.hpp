#pragma once

#include "XMLNode.hpp"
#include <map>

class XMLTree {
public:
	XMLTree(std::istream& input);

	void print(unsigned int tab_size = 2, unsigned int tabs = 0, std::ostream& out = std::cout) const;
private:
	XMLNode* root;
	enum class symbol_type {
		LESS_THAN,
		GREATER_THAN,
		SLASH,
		EQUALS,
		QUOTE,
		TEXT,
		SPACE
	};

	symbol_type evaluate_symbol(char symbol);

	void parse_node(std::istream& input, XMLNode* parent, std::map<std::string, int>& id_count);

	//char read_a_symbol(std::istream& input, char symbol);//delete maybe useless

	void match_closing_tag(std::istream& input, XMLNode* parent);
	
	XMLNode* read_tag_and_attributes(std::istream& input);

	std::string read_word(std::istream& input, char& last_read_symbol);

	std::string read_word_inside_quotes(std::istream& input);

	void read_attributes(std::istream& input, std::vector<std::string>& attributes_names, std::vector<std::string>& attributes_values, bool& self_closing);

	void skip_whitespaces(std::istream& input, char& last_read_symbol);
};