#include "XMLNode.hpp"

XMLNode::XMLNode(const std::string& tag, const std::vector<std::string>& attribute_names = {}, const std::vector<std::string>& attribute_values = {},
	const std::string& value = "")
	: value(value),
	tag(tag),
	attribute_names(attribute_names),
	attribute_values(attribute_values) {}

XMLNode::XMLNode(const XMLNode& other)
	: value(other.value),
	tag(other.tag),
	attribute_names(other.attribute_names),
	attribute_values(other.attribute_values) {

	for (XMLNode* child : other.children) {
		children.push_back(new XMLNode(*child));
	}
}

XMLNode& XMLNode::operator= (const XMLNode& other) {
	XMLNode copy(other);
	swap_Nodes(copy);
	return *this;
}

//XMLNode& XMLNode::add_child(const XMLNode* child) {
	
//}

XMLNode::~XMLNode() {
	for (int i = 0; i < children.size(); i++) {
		delete children[i];
	}
}

void XMLNode::swap_Nodes(XMLNode& other) {
	std::swap(tag, other.tag);
	std::swap(value, other.value);
	std::swap(attribute_names, other.attribute_names);
	std::swap(attribute_values, other.attribute_values);
	std::swap(children, other.children);
}