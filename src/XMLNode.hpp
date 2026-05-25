#pragma once
#include<iostream>
#include<string>
#include<vector>
#include<map>

class XMLNode {
public:

	XMLNode(const std::string& tag, const std::vector<std::string>& attribute_names = {}, const std::vector<std::string>& attribute_values = {},
		const std::string& value = "", bool self_closing = false);

	XMLNode(const XMLNode& other);

	XMLNode(XMLNode&& other) noexcept;// maybe useless

	XMLNode& operator= (XMLNode&& other) noexcept;//maybe useless

	XMLNode& operator= (const XMLNode& other);

	XMLNode& add_child(XMLNode* child);

	bool has_attribute_names(std::string name) const;

	std::string get_attribute_value(std::string attribute_name, bool& found_attribute) const;
	
	std::string get_tag() const;
	
	bool get_self_closing() const;
	
	void set_value(std::string a);

	void set_self_closing(bool a);

	void print(unsigned int tab_size = 2, unsigned int tabs = 0, std::ostream& out = std::cout) const;

	void make_unique_ids(std::map<std::string, int>& id_count);

	~XMLNode();
private:
	std::vector<XMLNode*> children;
	std::string value, tag;
	std::vector<std::string> attribute_names, attribute_values;
	bool self_closing;

	void swap_Nodes (XMLNode& other);

	void print_tag(std::ostream& out) const;
};