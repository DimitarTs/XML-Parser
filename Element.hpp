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
	void concatToID(const char*);
	void copyFrom(const Element& other);
public:
	friend void assignUniqueIDs(Element*, int);
	friend void readFromFile(const char*, Element*);
	friend void addNewElement(Element*&, Element, int&);
	Element();
	Element(const Element& other);
	Element& operator=(const Element& other);
	~Element();
	bool operator!=(Element other);
	void setID(const char*);
	const char* getID();
	void setTag(const char*);
	void setText(const char*);
	void removeText();
	void print(int);
	void addChild(Element&);
	void removeChild(Element&);
	void addAttribute(Attribute);
	void addAttribute(const char*, const char*);
};

void Element::concatToID(const char* suffix)
{
	char* newID = new char[strlen(id) + strlen(suffix) + 1];
	int i;
	for (i = 0; id[i] != '\0'; i++)
		newID[i] = id[i];
	for (int j = 0; suffix[j] != '\0'; j++, i++)
	{
		newID[i] = suffix[j];
	}
	newID[strlen(id) + strlen(suffix)] = '\0';
	delete[] id;
	id = newID;
}

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

void Element::setID(const char* id)
{
	if (this->id != nullptr)
		delete[] this->id;
	this->id = new char[strlen(id) + 1];
	strcpy(this->id, id);
}
const char* Element::getID()
{
	return id;
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

void Element::print(int numberOfIndentations = 0)
{
	for (int i = 0; i < numberOfIndentations; i++)
		cout << "     ";
	cout << "<" << tag;
	cout << " id=" << '"' << id << '"';
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

void assignUniqueIDs(Element* arr, int size)
{
	for (int i = 0; i < size; i++)
	{
		if (!strcmp(arr[i].id, "unknown"))
		{
			char startID[2] = "1";
			startID[0] = ((i+1) % 10) + '0';
			arr[i].setID(startID);
			bool isIdentical;
			do
			{
				isIdentical = true;
				for (int k = 0; k < size and isIdentical; k++)
				{
					if (k != i && !strcmp(arr[k].id, arr[i].id))
					{
						isIdentical = false;
					}
				}
				if (!isIdentical)
				{
					char addition[2] = "2";
					addition[0] = (i % 10) + '0';
					arr[i].concatToID(addition);
				}
			} while (!isIdentical);
		}
		else
		{
			int numberOfReiterations = 1;
			for (int j = i + 1; j < size; j++)
			{
				if (!strcmp(arr[i].id, arr[j].id))
				{
					bool isIdentical = false;
					numberOfReiterations++;
					while (!isIdentical)
					{
						char addition[2] = "2";
						addition[0] = (numberOfReiterations % 10) + '0';
						arr[j].concatToID(addition);
						isIdentical = true;
						for (int k = 0; k < size and isIdentical; k++)
						{
							if (k != j && !strcmp(arr[k].id, arr[j].id))
							{
								isIdentical = false;
							}
						}
					}
				}
			}
			if (numberOfReiterations > 1)
			{
				char addition[2] = "1";
				arr[i].concatToID(addition);
			}
		}
	}
}

void addNewElement(Element*& arr, Element extraElement, int& size)
{
	/*Element* newArr = new Element[size + 1];
	for (int i = 0; i < size; i++)
	{
		if (arr[i].parent != nullptr)
		{
			arr[i].parent->removeChild(arr[i]);
			arr[i].parent->addChild(newArr[i]);
		}
		newArr[i] = arr[i];
	}
	newArr[size - 1] = extraElement;
	if (size > 0)
		delete[] arr;
	size++;
	arr = newArr;*/
}

void readFromFile(const char* filename, Element* arr)
{
	ifstream f;
	f.open(filename);
	if (f.fail())
	{
		cout << "Error - file not found\n";
		return;
	}
	char check;
	f.get(check);
	if (check != '<')
	{
		cout << "Error - invalid file\n";
	}
	char rootTag[64];
	for (int i = 0; rootTag[i] != ' ' && i < 64; i++)
	{
		f.get(rootTag[i]);
		if (rootTag[i] == ' ')
		{
			rootTag[i] = '\0';
		}
	}
}