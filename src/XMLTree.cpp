#include "XMLTree.hpp"

//remake
//XMLTree::XMLTree(std::istream& input): root (parse_node(input)) {}

void XMLTree::parse_node(std::istream& input, XMLNode* parent) {
	char symbol;

	while (input.get(symbol)) {
		symbol_type type = evaluate_symbol(symbol);

		if (type == symbol_type::SPACE) {
			continue;
		}

		if (type == symbol_type::LESS_THAN) {
			XMLNode* new_child = read_tag_and_attributes(input);

			//check if next symbol is / if it closes our tag return the recursion
			parent->add_child(new_child);
			
		}

		//check_for_text_and_add_it_to_the_node

		//check for the closing of our node if it is closed return the recursion

		//make an exception
	}
}

XMLTree::symbol_type XMLTree::evaluate_symbol(char symbol) {
	if (symbol >= 0 && symbol <= 32) {
		return symbol_type::SPACE;
	}
	if (symbol == '<') {
		return symbol_type::LESS_THAN;
	}
	if (symbol == '>') {
		return symbol_type::GREATER_THAN;
	}
	if (symbol == '=') {
		return symbol_type::EQUALS;
	}
	if (symbol == '"') {
		return symbol_type::QUOTE;
	}
	if (symbol == '/') {
		return symbol_type::SLASH;
	}
	return symbol_type::TEXT;
}



//maybe add logic to check if the <> ends with /
XMLNode* XMLTree::read_tag_and_attributes(std::istream& input) {
	char symbol;
	std::string tag = read_word(input, symbol);

	symbol_type a = evaluate_symbol(symbol);
	//checks if tag is empty if the <> has ended and if the symbol after the tag is a space
	if (tag == "") {
			//throw an exception there is no tag
	}
	if (a == symbol_type::GREATER_THAN) {
		return new XMLNode(tag);
	}
	if (a != symbol_type::SPACE) {
		//throw an exception there is something else other than space after tag
	}
	
	std::vector<std::string> attribute_names, attribute_values;
	read_attributes(input, attribute_names, attribute_values);

	return new XMLNode(tag, attribute_names, attribute_values);
}

//maybe add logic to check if the <> ends with /
void XMLTree::read_attributes(std::istream& input, std::vector<std::string>& attribute_names, std::vector<std::string>& attribute_values) {
	size_t attribute_count = 0;
	char symbol;

	while (input.get(symbol)) {
		symbol_type a = evaluate_symbol(symbol);

		if (a == symbol_type::SPACE) {
			continue;
		}

		if (a == symbol_type::GREATER_THAN) {
			return;
		}

		if (a == symbol_type::TEXT) {
			std::string current_name;
			current_name.push_back(symbol);
			current_name.append(read_word(input, symbol));

			skip_whitespaces(input, symbol);
			a = evaluate_symbol(symbol);
			if (a != symbol_type::EQUALS) {
				//return exception = was expected after attribute name, but got a different special symbol instead
			}

			skip_whitespaces(input, symbol);
			a = evaluate_symbol(symbol);
			if (a != symbol_type::QUOTE) {
				//return exception " was expected after = , but got a different special symbol instead
			}
			attribute_names.push_back(current_name);
			attribute_values.push_back(read_word_inside_quotes(input));
			continue;
		}
		
		//return exception unexpected special symbol
	}
}

std::string XMLTree::read_word(std::istream& input, char& last_read_symbol) {
	std::string word;
	while (input.get(last_read_symbol)) {
		symbol_type a = evaluate_symbol(last_read_symbol);
		if (a != symbol_type::TEXT) {
			return word;
		}
		word.push_back(last_read_symbol);
	}
	return word;
}

std::string XMLTree::read_word_inside_quotes(std::istream& input) {
	std::string word;
	char symbol;
	while (input.get(symbol)) {
		symbol_type a = evaluate_symbol(symbol);

		if (a == symbol_type::QUOTE) {
			return word; 
		}

		if (a == symbol_type::LESS_THAN) {
			// throw exception < is forbidden inside quotes
		}

		word.push_back(symbol);
	}
	return word;
}

void XMLTree::skip_whitespaces(std::istream& input, char& last_read_symbol) {
	while (input.get(last_read_symbol)) {
		symbol_type a = evaluate_symbol(last_read_symbol);
		if (a != symbol_type::SPACE) {
			return ;
		}
	}
}

/*redo
char XMLTree::read_a_symbol(std::istream& input, char symbol) {
	while (!input.eof()) {
		char character;
		input.get(character);
		if (character == symbol) {
			return true;
		}
		if (!(character >= 0 && character <= 32)) {
			return false;
		}
	}
	return false;
}*/