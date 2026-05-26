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
			<< "  set <id> <key> <val> - Set/Modify attribute value\n"
			<< "  delete <id> <key> - Delete an attribute from an element\n"
			<< "  text <id>         - Get text content of an element\n"
			<< "  newchild <id>     - Add a new empty child to an element\n"
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
			XMLTree* dummy = new XMLTree();
			std::swap(dummy, current_tree);
			delete dummy;
		}
		current_filename.clear();
		std::cout << "File closed succesfully!\n\n";
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
	else if (action == "print") {
		if (current_tree == nullptr) {
			std::cout << "Error: No XML file loaded! Please use 'open' first.\n";
		}
		else {
			current_tree->print();
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

	XMLNode* node = current_tree->find_Node(target_id);

	if (node != nullptr) {
		bool attr_exists = node->set_attribute(target_key, target_value);

		if (attr_exists) {
			std::cout << "Changed succesfully: " << target_key << " = \"" << target_value << "\"\n\n";
		}
		else {
			std::cout << "Added successfully new attribute: " << target_key << " = \"" << target_value << "\"\n\n";
		}
	} else {
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