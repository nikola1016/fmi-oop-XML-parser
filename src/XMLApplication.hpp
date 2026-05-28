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

	void delete_attribute(const std::vector<std::string>& args);
	
	void get_text(const std::vector<std::string>& args) const;

	void add_new_child(const std::vector<std::string>& args);

	void children_of_id(const std::vector<std::string>& args) const;

	void print_child_by_id(const std::vector<std::string>& args) const;

	void print_file(const std::string& filename) const;

	void xpath_parser(const std::vector<std::string>& args) const;

	std::vector<XPathStep> convert_to_xpathstep(const std::string& xpath_message) const;

	bool read_brackets(const std::string& xpath_message, XPathStep& new_xpathstep) const;

	void print_xpath_result(const XPathStep& last_step, const std::vector<XMLNode*>& candidates) const;

	bool find_interval(const std::string& arg) const;

	bool is_pure_number(const std::string& str) const;
};