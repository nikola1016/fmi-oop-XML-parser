#pragma once
#include<iostream>
#include<string>
#include<vector>

class XMLNode {
public:

	XMLNode(const std::string& tag, const std::vector<std::string>& attribute_names = {}, const std::vector<std::string>& attribute_values = {},
		const std::string& value = "");

	XMLNode(const XMLNode& other);

	//TODO: move constructor for add_child

	XMLNode& operator= (const XMLNode& other);

	XMLNode& add_child(const XMLNode* child);

	~XMLNode();
private:
	std::vector<XMLNode*> children;
	std::string value, tag;
	std::vector<std::string> attribute_names, attribute_values;

	void swap_Nodes (XMLNode& other);
};