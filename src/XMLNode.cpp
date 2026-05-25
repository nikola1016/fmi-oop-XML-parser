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

bool XMLNode::has_attribute_names(std::string name) const {
	for (std::string attribute : attribute_names) {
		if (attribute == name) {
			return true;
		}
	}
	return false;
}

std::string XMLNode::get_attribute_value(std::string attribute_name, bool& found_attribute) const {
	int size = attribute_name.size();
	for (int i = 0; i < size; i++) {
		if (attribute_name == attribute_names[i]) {
			found_attribute = true;
			return attribute_values[i];
		}
	}
	found_attribute = false;
	return "";
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

void XMLNode::print(unsigned int tab_size, unsigned int tabs, std::ostream& out) const {
	std::string indent(tabs * tab_size, ' ');

	out << indent;
	print_tag(out);
	out << '\n';

	if (self_closing == true) {
		return;
	}
	
	for (XMLNode* child : children) {
		child->print(tab_size, tabs + 1, out);
	}

	if (value != "") {
		out << indent << std::string(tab_size, ' ');
		out << value;
		out << '\n';
	}

	out << indent;
	out << "</" << tag << ">";
	out << '\n';

	//check if its a self closing tag
	//if it is print and return
	//if its not print and open a for loop loping through all children and returning the recursion for each increasing the tabs
	//if we have a value print it but with one tab more
	//print end tag
}

void XMLNode::make_unique_ids(std::map<std::string, int>& id_count) {
	bool has_id_attribute = false;
	size_t id_index = 0;

	for (size_t i = 0; i < attribute_names.size(); ++i) {
		if (attribute_names[i] == "id") {
			has_id_attribute = true;
			id_index = i;
			break;
		}
	}

	if (has_id_attribute) {
		std::string& current_id = attribute_values[id_index];

		auto it = id_count.find(current_id);
		if (it != id_count.end()) {
			int value_count = it->second;

			if (value_count > 1) {
				int suffix = value_count - 1;
				it->second--; 

				current_id.append("_").append(std::to_string(suffix));
			}
		}
	} else {
		attribute_names.insert(attribute_names.begin(), "id");
		int failed_attempts = 1;
		std::string id_value;
		while (true) {
			id_value = "auto_gen_id_";
			id_value.append(std::to_string(id_count.size() + failed_attempts++));
			if (id_count.find(id_value) == id_count.end()) {
				break;
			}
		}
		id_count[id_value] = 1;
		attribute_values.insert(attribute_values.begin(), id_value);
	}

	for (XMLNode* child : children) {
		if (child != nullptr) {
			child->make_unique_ids(id_count);
		}
	}
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

void XMLNode::print_tag(std::ostream& out) const {
	out << '<' << tag;
	for (int i = 0; i < attribute_names.size(); i++) {
		out << ' ' << attribute_names[i] << " = \"" << attribute_values[i] << "\"";
	}
	if (self_closing == true) {
		out << '/';
	}
	out << '>';
}

