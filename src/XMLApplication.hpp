#pragma once
#include "XMLTree.hpp"

class XMLApplication {
public:
	XMLApplication();

	void run(); // не е готов
private:
	XMLTree* current_tree;
	std::string current_filename;

	void process_command(std::string& command_line);

	std::vector<std::string> split_args(std::string command_line) const;

	void open_file(const std::vector<std::string>& args);

	void save_file(const std::vector<std::string>& args);

	void select_attribute(const std::vector<std::string>& args) const;

	void set_attribute_value(const std::vector<std::string>& args);

	void print_file(const std::string& filename) const;
};