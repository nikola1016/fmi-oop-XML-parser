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

const std::vector<XMLNode*>& XMLNode::get_children() const {
	return children;
}

const std::vector<std::string>& XMLNode::get_attribute_names() const {
	return attribute_names;
}

bool XMLNode::has_attribute_names(std::string name) const {
	for (std::string attribute : attribute_names) {
		if (attribute == name) {
			return true;
		}
	}
	return false;
}

std::string XMLNode::get_text() const {
	return value;
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
	
	out << '\n';
	out << indent;
	print_tag(out);

	if (self_closing == true) {
		return;
	}
	
	for (XMLNode* child : children) {
		child->print(tab_size, tabs + 1, out);
	}

	if (!children.empty()) {
		out << '\n' << indent << "</" << tag << ">";
	}
	else {
		if (!value.empty()) {
			out << value; 
		}
		out << "</" << tag << ">"; 
	}

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

XMLNode* XMLNode::find_child_by_id(const std::string& target_id) {
	for (int i = 0; i < attribute_values.size(); i++) {
		if (attribute_names[i] == "id") {
			if (attribute_values[i] == target_id) {
				return this;
			}
			break;
		}
	}

	for (int i = 0; i < children.size(); i++) {
		if (children[i] != nullptr) {
			XMLNode* found = children[i]->find_child_by_id(target_id);
			if (found != nullptr) {
				return found; 
			}
		}
	}
	return nullptr;
}

bool XMLNode::delete_attribute(const std::string& key) {
	size_t size = attribute_names.size();
	for (int i = 0; i < size; i++) {
		if (attribute_names[i] == key) {
			attribute_names.erase(attribute_names.begin() + i);
			attribute_values.erase(attribute_values.begin() + i);
			return true;
		}
	}
	return false;
}

bool XMLNode::set_attribute(const std::string& key, const std::string& value) {
	size_t size = attribute_names.size();
	for (int i = 0; i < size; i++) {
		if (attribute_names[i] == key) {
			attribute_values[i] = value;
			return true;
		}
	}
	attribute_names.push_back(key);
	attribute_values.push_back(value);
	return false;
}

void XMLNode::execute_xpath(const std::vector<XPathStep>& xpath_steps,
	std::vector<XMLNode*>& candidates) const{
	
	size_t xpath_steps_size = xpath_steps.size();
	
	for (size_t i = 0; i < xpath_steps_size; i++) {
		//проверка дали [@somthing] стои някъде освен в края на xpath заявката
		if (!xpath_steps[i].extract_attribute.empty() &&
				!xpath_steps[i].has_condition && i != xpath_steps_size - 1) {
			std::cout << "Error: [@something] must be the last variable in the xpath!\n\n";
			candidates.clear();
			return;
		}//не се притеснявам за smth[@something] повече
		 //на последна позиция съм или взимам tag[]/
		if (i == 0 && candidates.size() == 1 && candidates[0]->tag == xpath_steps[i].tag_name) {}
		else {
			get_children_with_wanted_tag(candidates, xpath_steps[i].tag_name);
		}
		//ако няма деца прекратяваме цикъла
		if (candidates.empty()) {
			return;
		}
		//оправям се ако имам [index]
		if (xpath_steps[i].index != -1) {
			handle_xpath_indexes(candidates, xpath_steps[i].index);
		}
		if (xpath_steps[i].has_condition) {
			//оправяме случая с something[@id="idk"]
			if (!xpath_steps[i].extract_attribute.empty()) {
				filter_by_wanted_attr_name(candidates, xpath_steps[i].extract_attribute, xpath_steps[i].condition_value);
			}
			//оправяме случая с something[idk1="idk2"]
			else {
				filter_by_wanted_child_value(candidates, xpath_steps[i].condition_child_tag, xpath_steps[i].condition_value);
			}
		}
		//ако няма деца прекратяваме цикъла
		if (candidates.empty()) {
			return;
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

void XMLNode::get_children_with_wanted_tag(std::vector<XMLNode*>& candidates, const std::string& tag) const {
	size_t initial_candidate_size = candidates.size();
	for (size_t i = 0; i < initial_candidate_size; i++) {
		for (size_t j = 0; j < candidates[0]->children.size(); j++) {
			if (candidates[0]->children[j] != nullptr) {
				if (candidates[0]->children[j]->tag == tag) {
					candidates.push_back(candidates[0]->children[j]);
				}
			}
		}
		candidates.erase(candidates.begin());
	}
}

void XMLNode::handle_xpath_indexes(std::vector<XMLNode*>& candidates, size_t index) const {
	if (index < candidates.size()) {
		XMLNode* target = candidates[index];
		candidates.clear();                
		candidates.push_back(target);    
	}
	else {
		candidates.clear();                  
	}
}

void XMLNode::filter_by_wanted_attr_name(std::vector<XMLNode*>& candidates,
																				 const std::string& wanted_attribute_name,
																				 const std::string& wanted_attribute_value) const {

	for (size_t i = 0; i < candidates.size(); i++) {
		bool found_attribute;
		if (candidates[i]->get_attribute_value(wanted_attribute_name, found_attribute) == wanted_attribute_value) {
			continue;
		}
		candidates.erase(candidates.begin() + i);
		i--;
	}
}

void XMLNode::filter_by_wanted_child_value(std::vector<XMLNode*>& candidates,
																					 const std::string& wanted_child_tag,
																					 const std::string& wanted_value) const {
	bool found_wanted_child = false;
	for (size_t i = 0; i < candidates.size(); i++) {
		for (size_t j = 0; j < candidates[i]->children.size(); j++) {
			if (candidates[i]->children[j] != nullptr) {
				if (candidates[i]->children[j]->tag == wanted_child_tag &&
					candidates[i]->children[j]->value == wanted_value) {
					found_wanted_child = true;
					break;
				}
			}
		}
		if (!found_wanted_child) {
			candidates.erase(candidates.begin() + i);
			i--;
		}
		found_wanted_child = false;
	}
}