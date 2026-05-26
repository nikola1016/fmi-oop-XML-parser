#include "XMLTree.hpp"

XMLTree::XMLTree(): root(nullptr){}

XMLTree::XMLTree(std::istream& input) : root(nullptr) {
	char symbol;

	while (input.get(symbol)) {
		if (evaluate_symbol(symbol) == symbol_type::SPACE) {
			continue;
		}
		if (evaluate_symbol(symbol) == symbol_type::LESS_THAN) {
			break;
		}
		throw;// throw exception the file must start with <
	}

	root = read_tag_and_attributes(input);
	std::map<std::string, int> id_count;

	if (root != nullptr && !root->get_self_closing()) {
		parse_node(input, root, id_count);
	}

	if (root->has_attribute_names("id")) {
		bool found;
		id_count[root->get_attribute_value("id", found)]++;
	}
	
	for (auto& pair : id_count) {
		if (pair.second > 1) {
			pair.second++;
		}
	}

	root->make_unique_ids(id_count);
}

XMLTree::XMLTree(const XMLTree& other): root(nullptr) {
	if (other.root != nullptr) {
		this->root = new XMLNode(*(other.root));
	}
}

XMLTree& XMLTree::operator=(const XMLTree& other) {
	if (this != &other) {
		XMLTree temp(other);
		std::swap(this->root, temp.root);
	}
	return *this;
}

void XMLTree::print(unsigned int tab_size, unsigned int tabs, std::ostream& out) const {
	if (root != nullptr) {
		root->print(tab_size, tabs, out);
	}
	else {
		std::cout << "No XML tree saved\n\n";
	}
}

XMLNode* XMLTree::find_Node(std::string id) {
	return root->find_child_by_id(id);
}

XMLTree::~XMLTree() {
	delete root;
}

void XMLTree::parse_node(std::istream& input, XMLNode* parent, std::map<std::string, int>& id_count) {
	char symbol;

	while (input.get(symbol)) {
		symbol_type type = evaluate_symbol(symbol);

		if (type == symbol_type::SPACE) {
			continue;
		}

		if (type == symbol_type::LESS_THAN) {
			if (evaluate_symbol(input.peek()) == symbol_type::SLASH) {
				input.get(symbol);
				match_closing_tag(input, parent);
				return;
			}

			XMLNode* new_child = read_tag_and_attributes(input);
			if (new_child->has_attribute_names("id")) {
				bool found;
				id_count[new_child->get_attribute_value("id", found)]++;
			}

			if (!new_child->get_self_closing()) {
				parse_node(input, new_child, id_count);
			}

			if (parent != nullptr) {
				parent->add_child(new_child);
			}

			continue;
		}

		if (type == symbol_type::TEXT) {
			std::string message;
			message.push_back(symbol); 

			while (input.get(symbol)) {
				if (evaluate_symbol(symbol) == symbol_type::LESS_THAN) {
					break; 
				}
				message.push_back(symbol);
			}

			if (parent != nullptr) {
				parent->set_value(message);
			}

			if (evaluate_symbol(input.peek()) == symbol_type::SLASH) {
				input.get(symbol);
				match_closing_tag(input, parent);
				return;
			}

			continue;
		}

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

void XMLTree::match_closing_tag(std::istream& input, XMLNode* parent) {
	char symbol;
	std::string closing_tag = read_word(input, symbol);

	if (closing_tag.empty()) {
		throw std::runtime_error("Синтактична грешка: Намерена е празна затваряща скоба '</>'!");
	}

	if (evaluate_symbol(symbol) != symbol_type::GREATER_THAN) {
		throw std::runtime_error("Синтактична грешка: Очакван символ '>' след затварящия таг '" + closing_tag + "'!");
	}

	if (parent && closing_tag != parent->get_tag()) {
		throw std::runtime_error("Синтактична грешка: Несъответствие! Отварящият таг е <"
			+ parent->get_tag() + ">, но затварящият е </" + closing_tag + ">!");
	}
}

XMLNode* XMLTree::read_tag_and_attributes(std::istream& input) {
	char symbol;
	std::string tag = read_word(input, symbol);

	symbol_type a = evaluate_symbol(symbol);
	if (tag == "") {
		throw std::runtime_error("Синтактична грешка: Намерен е празен таг '<>'!");
	}
	if (a == symbol_type::SLASH) {
		if (evaluate_symbol(input.get()) == symbol_type::GREATER_THAN) {
			return new XMLNode(tag, {},{},"", true);
		}
		else {
			throw std::runtime_error("Синтактична грешка: Очакван символ '>' веднага след '/' в тага <" + tag + "/>!");
		}
	}
	if (a == symbol_type::GREATER_THAN) {
		return new XMLNode(tag);
	}
	if (a != symbol_type::SPACE) {
		throw std::runtime_error("Синтактична грешка: Невалиден символ веднага след името на тага <" + tag + ">!");
	}
	
	std::vector<std::string> attribute_names, attribute_values;
	bool self_closing = false;
	read_attributes(input, attribute_names, attribute_values, self_closing);

	return new XMLNode(tag, attribute_names, attribute_values, "", self_closing);
}

void XMLTree::read_attributes(std::istream& input, std::vector<std::string>& attribute_names, std::vector<std::string>& attribute_values,
	bool& self_closing) {

	size_t attribute_count = 0;
	char symbol;

	while (input.get(symbol)) {
		symbol_type a = evaluate_symbol(symbol);

		if (a == symbol_type::SPACE) {
			continue;
		}

		if (a == symbol_type::SLASH) {
			if (evaluate_symbol(input.get()) == symbol_type::GREATER_THAN) {
				self_closing = true;
				return ;
			}
			else {
				throw std::runtime_error("Синтактична грешка: Очакван символ '>' след '/' при затваряне на атрибутите!");
			}
		}

		if (a == symbol_type::GREATER_THAN) {
			return;
		}

		if (a == symbol_type::TEXT) {
			std::string current_name;
			current_name.push_back(symbol);
			current_name.append(read_word(input, symbol));

			a = evaluate_symbol(symbol);
			if (a == symbol_type::SPACE) {
				skip_whitespaces(input, symbol);
				a = evaluate_symbol(symbol);
				
			}

			if (a != symbol_type::EQUALS) {
				throw std::runtime_error("Синтактична грешка: Очакван символ '=' след името на атрибута '" + current_name + "'!");
			}

			skip_whitespaces(input, symbol);
			a = evaluate_symbol(symbol);
			if (a != symbol_type::QUOTE) {
				throw std::runtime_error("Синтактична грешка: Очаквана отваряща кавичка '\"' за стойността на атрибута '" + current_name + "'!");
			}
			attribute_names.push_back(current_name);
			attribute_values.push_back(read_word_inside_quotes(input));
			continue;
		}
		
		throw std::runtime_error("Синтактична грешка: Неочакван специален символ вътре в списъка с атрибути!");
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
			throw;// throw exception < is forbidden inside quotes
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