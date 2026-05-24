#include "XMLNode.hpp"

XMLNode::XMLNode(const std::string& tag, const std::vector<std::string>& attribute_names, const std::vector<std::string>& attribute_values,
	const std::string& value, bool self_closing)
	: value(value),
	tag(tag),
	attribute_names(attribute_names),
	attribute_values(attribute_values),
	self_closing(self_closing) {}

XMLNode::XMLNode(const XMLNode& other)
	: value(other.value),
	tag(other.tag),
	attribute_names(other.attribute_names),
	attribute_values(other.attribute_values),
  self_closing(other.self_closing) {

	for (XMLNode* child : other.children) {
		children.push_back(new XMLNode(*child));
	}
}

XMLNode::XMLNode(XMLNode&& other) noexcept
	: children(std::move(other.children)),
	value(std::move(other.value)),
	tag(std::move(other.tag)),
	attribute_names(std::move(other.attribute_names)),
	attribute_values(std::move(other.attribute_values)),
  self_closing(std::move(other.self_closing)) {}

XMLNode& XMLNode::operator= (XMLNode&& other) noexcept {
	if (this != &other) {
		swap_Nodes(other);
	}
	return *this;
}

XMLNode& XMLNode::operator= (const XMLNode& other) {
	XMLNode copy(other);
	swap_Nodes(copy);
	return *this;
}

XMLNode& XMLNode::add_child(XMLNode* child) {
	children.push_back(child);
	return *this;
}

std::string XMLNode::get_tag() const {
	return tag;
}

bool XMLNode::get_self_closing() const {
	return self_closing;
}

void XMLNode::set_value(std::string a) {
	value = a;
}

void XMLNode::set_self_closing(bool a) {
	self_closing = a;
}

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
	std::swap(self_closing, other.self_closing);
}