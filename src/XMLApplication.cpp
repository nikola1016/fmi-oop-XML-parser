#include "XMLApplication.hpp"
#include <fstream>

XMLApplication::XMLApplication() {
	current_tree = nullptr;
	current_filename = "";
}

void XMLApplication::run() {
	std::string command_line;

	std::cout << "--- XML Parser & XPath Engine Started ---\n";
	std::cout << "Type 'help' for a list of available commands.\n\n";

	while (true) {
		std::cout << "xml> ";
		std::getline(std::cin, command_line);

		if (command_line.empty()) {
			continue;
		}

		if (command_line == "exit") {
			std::cout << "Exiting program. Goodbye!\n";
			break;
		}

		process_command(command_line);
	}
}

void XMLApplication::process_command(std::string& command_line) {
	std::vector<std::string> args = split_args(command_line);
	if (args.empty()) return;

	std::string action = args[0];

	if (action == "help") {
		std::cout << "\nAvailable commands:\n"
			<< "ready  open <file>       - Open and parse an XML file\n"
			<< "ready  close             - Close the current file\n"
			<< "ready  save              - Save changes to the current file\n"
			<< "ready  saveas <file>     - Save the tree to a new file\n"
			<< "ready  print             - Display the formatted XML tree\n"
			<< "ready  select <id> <key> - Get attribute value by ID and key\n"
			<< "ready  set <id> <key> <val> - Set/Modify attribute value\n"
			<< "ready  delete <id> <key> - Delete an attribute from an element\n"
			<< "ready  text <id>         - Get text content of an element\n"
			<< "ready  children <id>       - Lists the attributes of all nested elements (children) for a given ID\n"
			<< "ready  child <id> <n>      - Accesses and displays the n-th child of the element with the given ID\n"
			<< "ready  newchild <parent_id> <tag_name> - Add a new empty child to an element\n"
			<< "  xpath <expr>      - Execute a simple XPath 2.0 query\n"
			<< "  exit              - Exit the application\n\n";
	}
	else if (action == "open") {
		open_file(args);
		return;
	}
	else if (action == "save" || action == "saveas") {
		save_file(args);
		return;
	}
	else if (action == "close") {
		if (current_tree != nullptr) {
			delete current_tree;
			current_tree = nullptr;
		}
		current_filename.clear();
		std::cout << "File closed successfully.\n\n";
		return;
	}
	else if (action == "select") {
		select_attribute(args);
		return;
	}
	else if (action == "set") {
		set_attribute_value(args);
		return;
	}
	else if (action == "delete") {
		delete_attribute(args);
		return;
	}
	else if (action == "text") {
		get_text(args);
		return;
	}
	else if (action == "newchild") {
		add_new_child(args);
		return;
	}
	else if (action == "children") {
		children_of_id(args);
		return;
	}
	else if (action == "child") {
		print_child_by_id(args);
		return;
	}
	else if (action == "print") {
		if (current_tree == nullptr) {
			std::cout << "Error: No XML file loaded! Please use 'open' first.\n";
		}
		else {
			current_tree->print();
			std::cout << '\n';
		}
		return;
	}
	else {
		std::cout << "Error: Unknown command '" << action << "'. Type 'help' for a list of commands.\n";
	}
}

std::vector<std::string> XMLApplication::split_args(std::string command_line) const {
	std::vector<std::string> split_args;
	std::string current_word;
	bool in_quotes = false;

	for (char symbol : command_line) {
		if (in_quotes == false) {
			if (symbol >= 0 && symbol <= 32) {
				if (current_word != "") {
					split_args.push_back(current_word);
					if (split_args.size() == 1) {
						if (split_args[0] == "xpath") {
							size_t spaces = 0;
							for (size_t i = split_args[0].size();i < command_line.size(); i++) {
								if (command_line[i] >= 0 && command_line[i] <= 32) {
									spaces++;
								}
								else {
									break;
								}
							}
							split_args.push_back(command_line.substr(split_args[0].size() + spaces));
							return split_args;
						}
					}
					current_word = "";
				}
				continue;
			}
			if (symbol == '\"') {
				in_quotes = true;
				continue;
			}
			current_word.push_back(symbol);
		}
		else {
			if (symbol == '\"') {
				in_quotes = false;
				split_args.push_back(current_word);
				current_word = "";
				continue;
			}
			current_word.push_back(symbol);
		}
	}

	if (current_word != "") {
		split_args.push_back(current_word);
	}

	return split_args;
}

void XMLApplication::open_file(const std::vector<std::string>& args) {
	if (args.size() < 2) {
		std::cout << "Error: Missing filename! Usage: open <filename>\n\n";
		return;
	}
	if (args.size() > 2) {
		std::cout << "Error: Too many arguments! Usage: open <filename>\n\n";
		return;
	}

	std::string filename = args[1];
	std::ifstream file(filename);

	if (!file.is_open()) {
		std::cout << "Error: File \"" << filename << "\" could not be found or opened.\n\n";
		return;
	}

	try {
		XMLTree* new_tree = new XMLTree(file);

		if (current_tree != nullptr) {
			delete current_tree;
		}
		current_tree = new_tree;
		current_filename = filename;

		std::cout << "Success: File \"" << filename << "\" loaded and validated successfully.\n\n";
	}
	catch (const std::exception& e) {
		std::cout << "\n[PARSING ERROR]\n";
		std::cout << "The file does not match the expected XML structure.\n";
		std::cout << "Details: " << e.what() << "\n";
		std::cout << "Returning to main menu. Please fix the file and try again.\n\n";
	}

	file.close();
}

void XMLApplication::save_file(const std::vector<std::string>& args) {
	if (current_tree == nullptr) {
		std::cout << "Error: No XML file loaded to save!\n\n";
		return;
	}

	size_t size = args.size();
	if (size == 1) {
		if (args[0] == "save") {
			if (current_filename.empty()) {
				std::cout << "Error: No file is currently open. Please use 'saveas <filename>'.\n\n";
				return;
			}
			print_file(current_filename);
			return;
		}
		else {
			std::cout << "Error: Missing filename! Usage: saveas <filename>\n\n";
			return;
		}
		
	}
	if (size > 2) {
		std::cout << "Error: Too many arguments! Usage: saveas <filename>\n\n";
		return;
	}
	if (size == 2) {
		print_file(args[1]);
		current_filename = args[1];
		return;
	}
}

void XMLApplication::select_attribute(const std::vector<std::string>& args) const {
	if (current_tree == nullptr) {
		std::cout << "Error: No XML file loaded!\n\n";
		return;
	}
	if (args.size() < 3) {
		std::cout << "Error: Missing arguments! Usage: select <id> <key>\n\n";
		return;
	}

	std::string target_id = args[1];
	std::string target_key = args[2];

	XMLNode* node = current_tree->find_Node(target_id);

	if (node != nullptr) {
		bool attr_exists = false;
		std::string value = node->get_attribute_value(target_key, attr_exists);

		if (attr_exists) {
			std::cout << target_key << " = \"" << value << "\"\n\n";
		}
		else {
			std::cout << "Error: Attribute '" << target_key << "' not found on element [" << target_id << "].\n\n";
		}
	}
	else {
		std::cout << "Error: Element with ID '" << target_id << "' not found.\n\n";
	}
}

void XMLApplication::set_attribute_value(const std::vector<std::string>& args) {
	if (current_tree == nullptr) {
		std::cout << "Error: No XML file loaded!\n\n";
		return;
	}
	if (args.size() < 4) {
		std::cout << "Error: Missing arguments! Usage: set <id> <key> <value>\n\n";
		return;
	}
	if (args.size() > 4) {
		std::cout << "Error: Too many arguments! Usage: set <id> <key> <value>\n\n";
		return;
	}

	std::string target_id = args[1];
	std::string target_key = args[2];
	std::string target_value = args[3];

	if (find_interval(target_key)) {
		std::cout << "Invalid key! Keys cannot contain spaces or control characters!\n\n";
		return;
	}

	XMLNode* node = current_tree->find_Node(target_id);

	if (node != nullptr) {
		bool attr_exists = node->set_attribute(target_key, target_value);

		if (attr_exists) {
			std::cout << "Changed successfully: " << target_key << " = \"" << target_value << "\"\n\n";
		}
		else {
			std::cout << "Added successfully new attribute: " << target_key << " = \"" << target_value << "\"\n\n";
		}
	} else {
		std::cout << "Error: Element with ID '" << target_id << "' not found.\n\n";
	}
}

void XMLApplication::delete_attribute(const std::vector<std::string>& args) {
	if (current_tree == nullptr) {
		std::cout << "Error: No XML file loaded!\n\n";
		return;
	}
	if (args.size() < 3) {
		std::cout << "Error: Missing arguments! Usage: delete <id> <key>\n\n";
		return;
	}
	if (args.size() > 3) {
		std::cout << "Error: Too many arguments! Usage: delete <id> <key>\n\n";
		return;
	}

	std::string target_id = args[1];
	std::string target_key = args[2];

	XMLNode* node = current_tree->find_Node(target_id);

	if (node != nullptr) {
		bool deleted_successfully = node->delete_attribute(target_key);

		if (deleted_successfully) {
			std::cout << "Deleted successfully: " << target_key << "\n\n";
		}
		else {
			std::cout << target_key << " was not found!\n\n";
		}
	}
	else {
		std::cout << "Error: Element with ID '" << target_id << "' not found.\n\n";
	}
}

void XMLApplication::get_text(const std::vector<std::string>& args) const{
	if (current_tree == nullptr) {
		std::cout << "Error: No XML file loaded!\n\n";
		return;
	}
	if (args.size() < 2) {
		std::cout << "Error: Missing arguments! Usage: text <id>\n\n";
		return;
	}
	if (args.size() > 2) {
		std::cout << "Error: Too many arguments! Usage: text <id>\n\n";
		return;
	}

	std::string target_id = args[1];

	XMLNode* node = current_tree->find_Node(target_id);

	if (node != nullptr) {
		std::string text = node->get_text();

		if (text.empty()) {
			std::cout << target_id << " does not have any text!\n\n";
		}
		else {
			std::cout << "Text found: "<< text << "\n\n";
		}
	}
	else {
		std::cout << "Error: Element with ID '" << target_id << "' not found.\n\n";
	}
}

void XMLApplication::add_new_child(const std::vector<std::string>& args) {
	if (current_tree == nullptr) {
		std::cout << "Error: No XML file loaded!\n\n";
		return;
	}
	if (args.size() < 3) {
		std::cout << "Error: Missing arguments! Usage: newchild <parent_id> <tag_name>\n\n";
		return;
	}
	if (args.size() > 3) {
		std::cout << "Error: Too many arguments! Usage: newchild <parent_id> <tag_name>\n\n";
		return;
	}

	std::string target_id = args[1];
	std::string new_tag = args[2];

	if (find_interval(new_tag)) {
		std::cout << "Invalid key! Keys cannot contain spaces or control characters!\n\n";
		return;
	}

	XMLNode* node = current_tree->find_Node(target_id);

	if (node != nullptr) {
		XMLNode* new_child = new XMLNode(new_tag);

		std::string unique_id = current_tree->get_next_available_id();

		new_child->set_attribute("id", unique_id);

		node->add_child(new_child);

		std::cout << "Success: New child added with unique ID: \"" << unique_id << "\" under parent [" << target_id << "].\n\n";
	}
	else {
		std::cout << "Error: Element with ID '" << target_id << "' not found.\n\n";
	}
}

void XMLApplication::children_of_id(const std::vector<std::string>& args) const{
	if (current_tree == nullptr) {
		std::cout << "Error: No XML file loaded!\n\n";
		return;
	}
	if (args.size() < 2) {
		std::cout << "Error: Missing arguments! Usage: children <id>\n\n";
		return;
	}
	if (args.size() > 2) {
		std::cout << "Error: Too many arguments! Usage: children <id>\n\n";
		return;
	}

	std::string target_id = args[1];

	XMLNode* node = current_tree->find_Node(target_id);

	if (node != nullptr) {
		std::vector<XMLNode*> children = node->get_children();
		std::vector<std::string> attribute_names;
		std::cout << "Children of element " << target_id << ":\n";
		for (XMLNode* child : children) {
			std::cout << "  -> <" << child->get_tag() << "> attributes: ";
			attribute_names = child->get_attribute_names();
			for (std::string name : attribute_names) {
				bool found;
				std::string value = child->get_attribute_value(name, found);
				if (found) {
					std::cout << name << "=\"" << value << "\" ";
				}
				else {
					std::cout << "\nError: There was a missmatch of data in the attribute names!\n";
					return;
				}
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
	else {
		std::cout << "Error: Element with ID '" << target_id << "' not found.\n\n";
	}
}

void XMLApplication::print_child_by_id(const std::vector<std::string>& args) const{
	if (current_tree == nullptr) {
		std::cout << "Error: No XML file loaded!\n\n";
		return;
	}
	if (args.size() < 3) {
		std::cout << "Error: Missing arguments! Usage: child <id> <n>\n\n";
		return;
	}
	if (args.size() > 3) {
		std::cout << "Error: Too many arguments! Usage: child <id> <n>\n\n";
		return;
	}

	std::string target_id = args[1];

	bool is_number = is_pure_number(args[2]);
	
	if (!is_number) {
		std::cout << "Error: Second argument needs to be a pure number! Usage: child <id> <n>\n\n";
		return;
	}

	int index = std::stoi(args[2]);

	XMLNode* node = current_tree->find_Node(target_id);

	if (node != nullptr) {
		std::vector<XMLNode*> children = node->get_children();

		if (index < 0 || static_cast<size_t>(index) >= children.size()) {
			std::cout << "Error: Index out of bounds! This element has only " << children.size() << " children.\n\n";
			return;
		}

		if (children[index] != nullptr) {
			children[index]->print(); 
			std::cout << "\n\n"; 
		}
		else {
			std::cout << "Error: The element at index " << index << " is a nullptr!\n\n";
		}
	}
	else {
		std::cout << "Error: Element with ID '" << target_id << "' not found.\n\n";
	}
}

void XMLApplication::print_file(const std::string& filename) const {
	std::ofstream file(filename);
	if (!file.is_open()) {
		std::cout << "Error: File \"" << filename << "\" could not be opened.\n\n";
		return;
	}
	int tab_size = 2;
	std::cout << "What tab size to you want: ";
	std::cin >> tab_size;
	std::cin.ignore(10000, '\n');
	std::cout << std::endl;
	current_tree->print(tab_size, 0, file);
	std::cout << "Saved successfully to \"" << filename << "\"!\n\n";
	file.close();
}

void XMLApplication::xpath_parser(const std::vector<std::string>& args)const {
	if (current_tree == nullptr) {
		std::cout << "Error: No XML file loaded!\n\n";
		return;
	}
	if (args.size() != 2) {
		std::cout << "Error: Something went wrong while reading your meassage!\n\n";
		return;
	}

	std::string xpath = args[1];
	std::vector<XPathStep> xpath_steps = convert_to_xpathstep(xpath);

	if (xpath_steps.empty()) {
		return;
	}

	//to be continued....................
}

std::vector<XPathStep> XMLApplication::convert_to_xpathstep(const std::string& xpath_message) const {
	std::vector<XPathStep> xpath_steps;
	std::string curr_word;
	size_t curr_xpath = 0;					
	XPathStep new_xpathstep;

	size_t xpath_message_size = xpath_message.size();
	for (size_t i = 0; i < xpath_message_size; i++) {
		size_t slash_pos = xpath_message.find('/', i);
		size_t open_bracket_pos = xpath_message.find('[', i);

		if (slash_pos == std::string::npos && open_bracket_pos == std::string::npos) {
			curr_word.append(xpath_message.substr(i));
			new_xpathstep.tag_name = curr_word;
			xpath_steps.push_back(new_xpathstep);
			return xpath_steps;
		}

		if (slash_pos == std::string::npos || slash_pos > open_bracket_pos) {
			curr_word.append(xpath_message.substr(i, open_bracket_pos - i));
			if (curr_word.empty()) {
				xpath_steps.clear();
				return xpath_steps;
			}
			new_xpathstep.tag_name = curr_word;
			i = open_bracket_pos;
			curr_word.clear();
			size_t close_bracket_pos = xpath_message.find(']', i);
			if (!read_brackets(xpath_message.substr(open_bracket_pos + 1, close_bracket_pos - open_bracket_pos - 1), new_xpathstep)) {
				std::cout << "Error: Wrong syntaxis in brackets!\n\n";
				xpath_steps.clear();
				return xpath_steps;
			}
			xpath_steps.push_back(new_xpathstep);
			new_xpathstep = {};
			i = close_bracket_pos;
			continue;
		}

		if (open_bracket_pos == std::string::npos || slash_pos < open_bracket_pos) {
			curr_word.append(xpath_message.substr(i, slash_pos - i));
			if (curr_word.empty()) {
				continue;
			}
			new_xpathstep.tag_name = curr_word;
			i = slash_pos;
			curr_word.clear();
			xpath_steps.push_back(new_xpathstep);
			new_xpathstep = {};
			continue;
		}
	}

	if (curr_word.empty() && !new_xpathstep.tag_name.empty()) {
		xpath_steps.push_back(new_xpathstep);
		new_xpathstep = {};
		curr_xpath++;
		return xpath_steps;
	}
	else if (!curr_word.empty() && new_xpathstep.tag_name.empty()) {
		new_xpathstep.tag_name = curr_word;
		curr_word.clear();
		xpath_steps.push_back(new_xpathstep);
		new_xpathstep = {};
		curr_xpath++;
		return xpath_steps;
	}

	return xpath_steps;
}

bool XMLApplication::read_brackets(const std::string& xpath_message, XPathStep& new_xpathstep) const {
	size_t at_symbol_pos = xpath_message.find('@');
	size_t equals_pos = xpath_message.find('=');

	if (at_symbol_pos == 0 && equals_pos == std::string::npos) {
		new_xpathstep.extract_attribute = xpath_message.substr(1);
		return true;
	}

	if (equals_pos != std::string::npos) {
		std::string tag_name = xpath_message.substr(0, equals_pos);
		if (tag_name[0] == '@') {
			new_xpathstep.extract_attribute = tag_name.substr(1);
		}
		else {
			new_xpathstep.condition_child_tag = tag_name;
		}
		new_xpathstep.has_condition = true;
		size_t first_quote_pos = xpath_message.find('\"', equals_pos),
					 second_quote_pos = xpath_message.find('\"', first_quote_pos + 1);
		std::string quote_string = xpath_message.substr(first_quote_pos + 1, second_quote_pos - first_quote_pos - 1);
		new_xpathstep.condition_value = quote_string;
		return true;
	}

	if (is_pure_number(xpath_message)) {
		new_xpathstep.index = std::stoi(xpath_message);
		return true;
	}

	return false;
}

bool XMLApplication::find_interval(const std::string& arg) const {
	for (const char symbol : arg) {
		if (symbol >= 0 && symbol <= 32) {
			return true;
		}
	}
	return false;
}

bool XMLApplication::is_pure_number(const std::string& str) const {
	if (str.empty()) return false;

	for (char symbol : str) {
		if (symbol < '0' || symbol > '9') {
			return false;
		}
	}
	return true;
}
