#pragma once
#include<iostream>
#include<string>
#include<vector>
#include<map>

struct XPathStep {
	std::string tag_name;
	int index = -1;
	std::string extract_attribute;
	std::string condition_child_tag;
	std::string condition_value;
	bool has_condition = false;
};

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

	const std::vector<XMLNode*>& get_children() const;

	const std::vector<std::string>& get_attribute_names() const;

	std::string get_text() const;

	std::string get_attribute_value(std::string attribute_name, bool& found_attribute) const;
	
	std::string get_tag() const;
	
	bool get_self_closing() const;
	
	void set_value(std::string a);

	void set_self_closing(bool a);

	void print(unsigned int tab_size = 2, unsigned int tabs = 0, std::ostream& out = std::cout) const;

	void make_unique_ids(std::map<std::string, int>& id_count);

	XMLNode* find_child_by_id(const std::string& target_id);

	bool delete_attribute(const std::string& key);

	bool set_attribute(const std::string& key, const std::string& value);

	void execute_xpath(const std::vector<XPathStep>& xpath_steps,
		std::vector<XMLNode*>& candidates) const;

	~XMLNode();
private:
	std::vector<XMLNode*> children;
	std::string value, tag;
	std::vector<std::string> attribute_names, attribute_values;
	bool self_closing;

	void swap_Nodes (XMLNode& other);

	void print_tag(std::ostream& out) const;

	void get_children_with_wanted_tag(std::vector<XMLNode*>& candidates, const std::string& tag) const;

	void handle_xpath_indexes(std::vector<XMLNode*>& candidates, size_t index) const;
	
	void filter_by_wanted_attr_name(std::vector<XMLNode*>& candidates, 
																	const std::string& wanted_attribute_name, 
																	const std::string& wanted_attribute_value) const;

	void filter_by_wanted_child_value(std::vector<XMLNode*>& candidates,
																	  const std::string& wanted_child_tag,
																	  const std::string& wanted_value) const;
};