#include "Attribute.hpp"

class Element
{
	int numberOfAttributes;
	int numberOfChildren;
	char* tag;
	char* id;
	char* text;
	Attribute* attributes;
	Element* parent;
	Element** children;

	void copyFrom(const Element& other);
public:
	Element();
	Element(const Element& other);
	Element& operator=(const Element& other);
	~Element();
	bool operator!=(Element other);
	void setTag(const char*);
	void setText(const char*);
	void removeText();
	void print(int);
	void addChild(Element&);
	void removeChild(Element&);
	void addAttribute(Attribute);
	void addAttribute(const char*, const char*);
};

void Element::copyFrom(const Element& other)
{
	if (tag != nullptr)
		delete[] tag;
	tag = new char[strlen(other.tag) + 1];
	strcpy(tag, other.tag);

	if (id != nullptr)
		delete[] id;
	id = new char[strlen(other.id) + 1];
	strcpy(id, other.id);

	if (text != nullptr)
		delete[] text;
	if (other.text != nullptr)
	{
		text = new char[strlen(other.text) + 1];
		strcpy(text, other.text);
	}

	numberOfAttributes = other.numberOfAttributes;
	if (attributes != nullptr)
		delete[] attributes;
	if (numberOfAttributes > 0)
	{
		attributes = new Attribute[numberOfAttributes];
		for (int i = 0; i < numberOfAttributes; i++)
			attributes[i] = other.attributes[i];
	}

	parent = other.parent;

	numberOfChildren = other.numberOfChildren;
	if (children != nullptr)
		delete[] children;
	if (numberOfChildren > 0)
	{
		children = new Element*[numberOfChildren];
		for (int i = 0; i < numberOfChildren; i++)
			children[i] = other.children[i];
	}
}
Element::Element()
{
	numberOfAttributes = 0;
	numberOfChildren = 0;
	tag = new char[8];
	strcpy(tag, "unknown");
	id = new char[8];
	strcpy(id, "unknown");
	text = nullptr;
	attributes = nullptr;
	parent = nullptr;
	children = nullptr;
}
Element::Element(const Element& other)
{
	tag = id = text = nullptr;
	parent = nullptr;
	children = nullptr;
	attributes = nullptr;
	copyFrom(other);
}
Element& Element::operator=(const Element& other)
{
	copyFrom(other);
	return *this;
}
Element::~Element()
{
	delete[] id;
	if (text != nullptr)
		delete[] text;
	if (numberOfAttributes > 0)
		delete[] attributes;
	if(numberOfChildren > 0)
		delete[] children;
	delete[] tag;
}

bool Element::operator!=(Element other)
{
	if (strcmp(tag, other.tag))
		return true;
	if (strcmp(id, other.id))
		return true;
	return false;
}

void Element::setTag(const char* tag)
{
	if (this->tag != nullptr)
		delete[] this->tag;
	this->tag = new char[strlen(tag) + 1];
	strcpy(this->tag, tag);
}
void Element::setText(const char* text)
{
	if (this->text != nullptr)
		delete[] this->text;
	int textLen = strlen(text) + 1;
	this->text = new char[textLen];
	strcpy(this->text, text);
}
void Element::removeText()
{
	if (text != nullptr)
		delete[] text;
	text = nullptr;
}
void Element::addChild(Element& child)
{
	if (child.parent != nullptr)
	{
		child.parent->removeChild(child);
	}
	Element** newChildren;
	newChildren = new Element * [numberOfChildren + 1];
	for (int i = 0; i < numberOfChildren; i++)
		newChildren[i] = children[i];
	newChildren[numberOfChildren] = &child;
	if (numberOfChildren > 0)
		delete[] children;
	numberOfChildren++;
	children = newChildren;
	child.parent = this;
}
void Element::removeChild(Element& child)
{
	if (child.parent != this)
		return;
	if (numberOfChildren == 1)
	{
		numberOfChildren = 0;
		delete[] children;
		children = nullptr;
		child.parent = nullptr;
		return;
	}
	Element** newChildren;
	newChildren = new Element * [numberOfChildren - 1];
	int j = 0;
	for (int i = 0; i < numberOfChildren; i++)
	{
		if (*(children[i]) != child)
		{
			newChildren[j] = children[i];
			j++;
		}
	}
	numberOfChildren--;
	children = newChildren;
	child.parent = nullptr;
}
void Element::addAttribute(Attribute attribute)
{
	Attribute* newAttributes;
	newAttributes = new Attribute[numberOfAttributes + 1];
	for (int i = 0; i < numberOfAttributes; i++)
	{
		newAttributes[i] = attributes[i];
	}
	newAttributes[numberOfAttributes] = attribute;
	if (numberOfAttributes > 0)
		delete[] attributes;
	numberOfAttributes++;
	attributes = newAttributes;
}
void Element::addAttribute(const char* name, const char* value)
{
	Attribute attribute(name, value);
	if (strcmp(name, "unknown") and strcmp(value, "unknown"))
		addAttribute(attribute);
}

void Element::print(int numberOfIndentations)
{
	for (int i = 0; i < numberOfIndentations; i++)
		cout << "     ";
	cout << "<" << tag;
	for (int i = 0; i < numberOfAttributes; i++)
	{
		cout << " ";
		attributes[i].print();
	}
	cout << ">";
	if (text != nullptr)
	{
		if (numberOfChildren > 0)
		{
			cout << endl;
			for (int i = 0; i < numberOfIndentations + 1; i++)
				cout << "     ";
		}
		cout << text;
	}
	if (numberOfChildren > 0)
		cout << endl;
	for (int i = 0; i < numberOfChildren; i++)
	{
		children[i]->print(numberOfIndentations + 1);
	}
	if(numberOfChildren>0)
		for (int i = 0; i < numberOfIndentations; i++)
			cout << "     ";
	cout << "</" << tag << ">\n";
}