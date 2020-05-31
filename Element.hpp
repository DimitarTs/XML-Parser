#include "Attribute.hpp"

class ListOfIDs
{
	int numberOfIDs;
	char** ids;
public:
	ListOfIDs();
	ListOfIDs(const ListOfIDs&);
	~ListOfIDs();
	ListOfIDs& operator=(const ListOfIDs&);

	bool isTaken(const char*) const;

	void addID(const char*);
	void removeID(const char*);

	const char* getFriend(int index) const
	{
		if (index < 0 or index >= numberOfIDs)
			index = 0;
		return ids[index];
	}
	const char* operator[](int index) const
	{
		if (index < 0 or index >= numberOfIDs)
			index = 0;
		return ids[index];
	}
	int getnumberOfIDs() const { return numberOfIDs; }

	//void print() const;
};
ListOfIDs::ListOfIDs() : numberOfIDs(0), ids(nullptr) {}
ListOfIDs::ListOfIDs(const ListOfIDs& other) : numberOfIDs(other.numberOfIDs)
{
	if (numberOfIDs > 0)
	{
		ids = new char* [numberOfIDs];
		for (int i = 0; i < numberOfIDs; i++)
		{
			ids[i] = new char[strlen(other.ids[i]) + 1];
			strcpy(ids[i], other.ids[i]);
		}
	}
	else ids = nullptr;
}
ListOfIDs::~ListOfIDs()
{
	if (ids != nullptr)
	{
		for (int i = 0; i < numberOfIDs; i++)
			delete[] ids[i];
		delete[] ids;
	}
}
ListOfIDs& ListOfIDs::operator=(const ListOfIDs& other)
{
	if (this == &other)
		return *this;
	if (ids != nullptr)
	{
		for (int i = 0; i < numberOfIDs; i++)
			delete[] ids[i];
		delete[] ids;
	}

	numberOfIDs = other.numberOfIDs;
	if (numberOfIDs > 0)
	{
		ids = new char* [numberOfIDs];
		for (int i = 0; i < numberOfIDs; i++)
		{
			ids[i] = new char[strlen(other.ids[i]) + 1];
			strcpy(ids[i], other.ids[i]);
		}
	}
	else ids = nullptr;

	return *this;
}

bool ListOfIDs::isTaken(const char* other) const
{
	for (int i = 0; i < numberOfIDs; i++)
		if (!strcmp(ids[i], other))
			return true;
	return false;
}

void ListOfIDs::addID(const char* other)
{
	if (isTaken(other))
		return;
	char** newids = new char* [numberOfIDs + 1];
	for (int i = 0; i < numberOfIDs; i++)
		newids[i] = ids[i];
	newids[numberOfIDs] = new char[strlen(other) + 1];
	strcpy(newids[numberOfIDs], other);
	numberOfIDs++;
	delete[] ids;
	ids = newids;
}
void ListOfIDs::removeID(const char* other)
{
	if (!isTaken(other))
		return;
	if (numberOfIDs == 1)
	{
		delete[] ids[0];
		delete[] ids;
		ids = nullptr;
		numberOfIDs = 0;
		return;
	}

	bool found = false;
	char** newids = new char* [numberOfIDs - 1];
	for (int i = 0; i < numberOfIDs; i++)
	{
		if (!strcmp(other, ids[i]))
		{
			found = true;
			delete[] ids[i];
		}
		else if (!found)
			newids[i] = ids[i];
		else
		{
			newids[i - 1] = ids[i];
		}
	}
	delete[] ids;
	ids = newids;
	numberOfIDs--;
}

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
	void popID();
	void makeIDUnique(ListOfIDs&);
	void copyFrom(const Element& other);
public:
	/*friend void assignUniqueIDs(Element**, int);
	friend int readElementData(const char*, Element&);
	friend bool readFromFile(const char* filename, Element**& arr, int& size);
	friend void addNewElement(Element**&, Element, int&);
	friend bool removeElement(Element**&, Element*, int&);*/
	friend Element* findElementById(Element**, int, const char*);
	Element* findElementById(const char* id);
	Element();
	Element(const char*, const char*);
	Element(const Element& other);
	Element& operator=(const Element& other);
	~Element();
	bool operator!=(const Element other) const;
	bool operator==(const Element other) const;
	void setID(const char*);
	const char* getID() const;
	const char* getTag() const;
	const char* getText() const;
	void setTag(const char*);
	void setText(const char*);
	void removeText();
	void print(int, bool);
	void printToFile(ofstream&, int, bool);
	void addChild(Element);
	void removeChild(Element);
	void addAttribute(Attribute);
	void addAttribute(const char*, const char*);
	bool removeAttribute(const char*);
	const char* getAttributeValue(const char*) const;
	bool setAttributeValue(const char*, const char*);
	Element** const getChildren() const;
	Element* getParent() const;
	int getNumberOfChildren() const;
	const Attribute* getAttributes() const;
	int getNumberOfAttributes() const;
	int readElementData(const char*);
	bool writeToFile(const char* filename);
	bool readFromFile(const char* filename);
	void assignUniqueIDs(ListOfIDs&);
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
void Element::popID()
{
	if (strlen(id) == 1)
	{
		/*delete[] id;
		id = new char[8];
		strcpy(id, "unknown");*/
		return;
	}
	char* newID = new char[strlen(id)];
	for (int i = 0; id[i + 1] != '\0'; i++)
	{
		newID[i] = id[i];
	}
	newID[strlen(id) - 1] = '\0';
	delete[] id;
	id = newID;
}

void Element::makeIDUnique(ListOfIDs& mainList)
{
	ListOfIDs explore;
	explore.addID(id);
	for (int i = 0; i < explore.getnumberOfIDs(); i++)
	{
		setID(explore[i]);
		if (!mainList.isTaken(id))
		{
			return;
		}
		for (int i = 0; i < 10; i++)
		{
			char suffix[2] = { i + '0', '\0' };
			concatToID(suffix);
			explore.addID(id);
			popID();
		}
	}
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
Element::Element(const char* tag, const char* id = "unknown")
{
	this->tag = new char[strlen(tag) + 1];
	strcpy(this->tag, tag);

	this->id = new char[strlen(id) + 1];
	strcpy(this->id, id);

	numberOfAttributes = 0;
	numberOfChildren = 0;
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

bool Element::operator!=(const Element other) const
{
	if (strcmp(id, other.id))
		return true;
	if (strcmp(id, "unknown"))
		return false;
	if (strcmp(tag, other.tag))
		return true;
	if (strcmp(text, other.text))
		return true;;
	if (parent != other.parent)
		return true;
	if (numberOfChildren != other.numberOfChildren or numberOfAttributes != other.numberOfAttributes)
		return true;
	return false;
}

bool Element::operator==(const Element other) const
{
	if (strcmp(id, other.id))
		return false;
	if (strcmp(id, "unknown"))
		return true;
	if (strcmp(tag, other.tag))
		return false;
	if (strcmp(text, other.text))
		return false;
	if (parent != other.parent)
		return false;
	if (numberOfChildren != other.numberOfChildren or numberOfAttributes != other.numberOfAttributes)
		return false;
	return true;
}

void Element::setID(const char* id)
{
	if (this->id != nullptr)
		delete[] this->id;
	this->id = new char[strlen(id) + 1];
	strcpy(this->id, id);
}
const char* Element::getID() const
{
	return id;
}
const char* Element::getTag() const
{
	return tag;
}
void Element::setTag(const char* tag)
{
	if (this->tag != nullptr)
		delete[] this->tag;
	this->tag = new char[strlen(tag) + 1];
	strcpy(this->tag, tag);
}
const char* Element::getText() const
{
	return text;
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
void Element::addChild(Element child)
{
	if (child.parent != nullptr)
	{
		child.parent->removeChild(child);
	}
	Element** newChildren;
	newChildren = new Element * [numberOfChildren + 1];
	for (int i = 0; i < numberOfChildren; i++)
		newChildren[i] = children[i];
	newChildren[numberOfChildren] = new Element(child);
	newChildren[numberOfChildren]->parent = this;
	if (numberOfChildren > 0)
		delete[] children;
	numberOfChildren++;
	children = newChildren;
	child.parent = this;
}
void Element::removeChild(Element child)
{
	if (child.parent != this)
		return;
	if (numberOfChildren == 1)
	{
		numberOfChildren = 0;
		delete children[0];
		delete[] children;
		children = nullptr;
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
		else
		{
			delete children[i];
		}
	}
	delete[] children;
	numberOfChildren--;
	children = newChildren;
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

bool Element::removeAttribute(const char* key)
{
	int index = -1;
	for(int i = 0; i<numberOfAttributes and index == -1; i++)
		if (!strcmp(attributes[i].getName(), key))
		{
			index = i;
		}
	if (index == -1 or numberOfAttributes == 0)
		return false;
	if (numberOfAttributes == 1)
	{
		numberOfAttributes = 0;
		delete[] attributes;
		attributes = nullptr;
		return true;
	}
	Attribute* newAttributes;
	newAttributes = new Attribute[numberOfAttributes - 1];
	for (int i = 0; i < numberOfAttributes; i++)
	{
		if (i < index)
			newAttributes[i] = attributes[i];
		else if (i > index)
			newAttributes[i - 1] = attributes[i];
	}
	delete[] attributes;
	attributes = newAttributes;
	numberOfAttributes--;
	return true;
}

void Element::print(int numberOfIndentations = 0, bool hideIDs = false)
{
	for (int i = 0; i < numberOfIndentations; i++)
		cout << (char)9;
	cout << "<" << tag;
	if (strcmp(id, "unknown") and !hideIDs)
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
				cout << (char)9;
		}
		cout << text;
	}
	if (numberOfChildren > 0)
		cout << endl;
	for (int i = 0; i < numberOfChildren; i++)
	{
		children[i]->print(numberOfIndentations + 1, hideIDs);
	}
	if(numberOfChildren>0)
		for (int i = 0; i < numberOfIndentations; i++)
			cout << (char)9;
	cout << "</" << tag << ">\n";
}

void Element::printToFile(ofstream& f, int numberOfIndentations = 0, bool hideIDs = false)
{
	for (int i = 0; i < numberOfIndentations; i++)
		f << (char)9;
	f << "<" << tag;
	if(strcmp(id, "unknown") and !hideIDs)
		f << " id=" << '"' << id << '"';
	for (int i = 0; i < numberOfAttributes; i++)
	{
		f << " ";
		attributes[i].printToFile(f);
	}
	f << ">";
	if (text != nullptr)
	{
		if (numberOfChildren > 0)
		{
			f << endl;
			for (int i = 0; i < numberOfIndentations + 1; i++)
				f << (char)9;
		}
		f << text;
	}
	if (numberOfChildren > 0)
		f << endl;
	for (int i = 0; i < numberOfChildren; i++)
	{
		children[i]->printToFile(f, numberOfIndentations + 1, hideIDs);
	}
	if (numberOfChildren > 0)
		for (int i = 0; i < numberOfIndentations; i++)
			f << (char)9;
	f << "</" << tag << ">\n";
}

const char* Element::getAttributeValue(const char* key) const
{
	for(int i = 0; i< numberOfAttributes; i++)
		if (!strcmp(attributes[i].getName(), key))
		{
			return attributes[i].getValue();
		}
	return nullptr;
}

bool Element::setAttributeValue(const char* key, const char* value)
{
	for (int i = 0; i < numberOfAttributes; i++)
		if (!strcmp(attributes[i].getName(), key))
		{
			attributes[i].setValue(value);
			return true;
		}
	return false;
}

Element**const Element::getChildren() const
{
	return children;
}

Element* Element::getParent() const
{
	return parent;
}

int Element::getNumberOfChildren() const
{
	return numberOfChildren;
}

const Attribute* Element::getAttributes() const
{
	return attributes;
}

int Element::getNumberOfAttributes() const
{
	return numberOfAttributes;
}

//void assignUniqueIDs(Element** arr, int size)
//{
//	for (int i = 0; i < size; i++)
//	{
//		if (!strcmp(arr[i]->id, "unknown"))
//		{
//			char startID[2] = "1";
//			startID[0] = ((i+1) % 10) + '0';
//			arr[i]->setID(startID);
//			bool isIdentical;
//			do
//			{
//				isIdentical = true;
//				for (int k = 0; k < size and isIdentical; k++)
//				{
//					if (k != i && !strcmp(arr[k]->id, arr[i]->id))
//					{
//						isIdentical = false;
//					}
//				}
//				if (!isIdentical)
//				{
//					char addition[2] = "2";
//					addition[0] = (i % 10) + '0';
//					arr[i]->concatToID(addition);
//				}
//			} while (!isIdentical);
//		}
//		else
//		{
//			int numberOfReiterations = 1;
//			for (int j = i + 1; j < size; j++)
//			{
//				if (!strcmp(arr[i]->id, arr[j]->id))
//				{
//					bool isIdentical = false;
//					numberOfReiterations++;
//					while (!isIdentical)
//					{
//						char addition[2] = "2";
//						addition[0] = (numberOfReiterations % 10) + '0';
//						arr[j]->concatToID(addition);
//						isIdentical = true;
//						for (int k = 0; k < size and isIdentical; k++)
//						{
//							if (k != j && !strcmp(arr[k]->id, arr[j]->id))
//							{
//								isIdentical = false;
//							}
//						}
//					}
//				}
//			}
//			if (numberOfReiterations > 1)
//			{
//				char addition[2] = "1";
//				arr[i]->concatToID(addition);
//			}
//		}
//	}
//}

//void addNewElement(Element**& arr, Element extraElement, int& size)
//{
//	Element** newArr = new Element * [size + 1];
//	for (int i = 0; i < size; i++)
//	{
//		newArr[i] = arr[i];
//	}
//	newArr[size] = new Element(extraElement);
//	if (size > 0)
//		delete[] arr;
//	size++;
//	arr = newArr;
//}

//bool removeElement(Element**& arr, Element* toRemove, int& size)
//{
//	if (size == 0)
//		return false;
//	int index = -1;
//	for (int i = 0; i < size and index == -1; i++)
//		if (arr[i] == toRemove)
//			index = i;
//	if (index == -1)
//		return false;
//	if (toRemove->parent != nullptr)
//		toRemove->parent->removeChild(*toRemove);
//	int numberOfChildren = toRemove->numberOfChildren;
//	for (int i = 0; i < numberOfChildren; i++)
//	{
//		removeElement(arr, toRemove->children[0], size);
//	}
//	if (size == 1)
//	{
//		delete arr[0];
//		delete[] arr;
//		arr = nullptr;
//		size = 0;
//		return true;
//	}
//	Element** newArr = new Element * [size - 1];
//	for (int i = 0; i < size; i++)
//	{
//		if (i < index)
//			newArr[i] = arr[i];
//		else if (i > index)
//			newArr[i - 1] = arr[i];
//	}
//	delete[] arr;
//	delete toRemove;
//	size--;
//	arr = newArr;
//	return true;
//}

//int readElementData(const char* source, Element& result) // returns -1 if source is invalid, 0 if the element isn't closed in the same line and 1 if it is
//{
//	char tag[TAG_LIMIT];
//	int index = 0;
//	while (source[index] == ' ' or source[index] == 9) index++;
//	if(source[index++] != '<')
//		return -1;
//	extractWord(source, tag, index, TAG_LIMIT, false, '>');
//	result.setTag(tag);
//	while (source[index] != '>')
//	{
//		bool isId = false;
//		char key[KEY_LIMIT], value[VALUE_LIMIT];
//		extractWord(source, key, index, KEY_LIMIT, false, '=');
//		//cout << attribute << endl;
//		if (!strcmp(key, "id"))
//			isId = true;
//		while (source[index - 1] != '=') index++;
//		extractWord(source, value, index, VALUE_LIMIT, false, '"');
//		//cout << value << endl;
//		if (isId)
//		{
//			result.setID(value);
//		}
//		else
//		{
//			Attribute x(key, value);
//			result.addAttribute(x);
//		}
//		index++;
//	}
//	if (source[++index] == '\0')
//		return 0;
//	if (source[index] != '<')
//	{
//		char text[TEXT_LIMIT];
//		for (int i = 0; source[index] != '<' and source[index] != '\0'; i++, index++)
//		{
//			text[i] = source[index];
//			if (source[index + 1] == '<' or source[index + 1] == '\0')
//			{
//				text[i + 1] = '\0';
//			}
//		}
//		result.setText(text);
//	}
//	if (source[index] == '<' and source[index + 1] == '/')
//	{
//		index += 2;
//		extractWord(source, tag, index, TAG_LIMIT, false, '>');
//		if (strcmp(result.tag, tag))
//			return -1;
//		else
//			return 1;
//	}
//	return -1;
//}

int Element::readElementData(const char* source) // returns -1 if source is invalid, 0 if the element isn't closed in the same line and 1 if it is
{
	char tag[TAG_LIMIT];
	int index = 0;
	while (source[index] == ' ' or source[index] == 9) index++;
	if (source[index++] != '<')
		return -1;
	extractWord(source, tag, index, TAG_LIMIT, false, '>');
	setTag(tag);
	while (source[index] != '>')
	{
		bool isId = false;
		char key[KEY_LIMIT], value[VALUE_LIMIT];
		extractWord(source, key, index, KEY_LIMIT, false, '=');
		if (!strcmp(key, "id"))
			isId = true;
		while (source[index - 1] != '=') index++;
		extractWord(source, value, index, VALUE_LIMIT, false, '"');
		if (isId)
		{
			setID(value);
		}
		else
		{
			addAttribute(key, value);
		}
		index++;
	}
	if (source[++index] == '\0')
		return 0;
	if (source[index] != '<')
	{
		char text[TEXT_LIMIT];
		for (int i = 0; source[index] != '<' and source[index] != '\0'; i++, index++)
		{
			text[i] = source[index];
			if (source[index + 1] == '<' or source[index + 1] == '\0')
			{
				text[i + 1] = '\0';
			}
		}
		setText(text);
	}
	if (source[index] == '<' and source[index + 1] == '/')
	{
		index += 2;
		extractWord(source, tag, index, TAG_LIMIT, false, '>');
		if (strcmp(this->tag, tag))
		{
			return -1;
		}
		else
		{
			return 1;
		}
	}
	return -1;
}

bool Element::readFromFile(const char* filename)
{
	ifstream f;
	f.open(filename);
	if (f.fail())
	{
		cout << "Error, file failed to open\n";
		f.close();
		return false;
	}
	char fileLine[INPUT_LIMIT];
	f.getline(fileLine, INPUT_LIMIT);
	int status = readElementData(fileLine);
	if (status == -1)
	{
		return false;
	}
	Element* current = this;
	bool rootIsClosed = (status == 1);
	while (!rootIsClosed)
	{
		f.getline(fileLine, INPUT_LIMIT);
		int lineIndex = 0;
		char check[TAG_LIMIT + 3];
		extractWord(fileLine, check, lineIndex, TAG_LIMIT + 3, true);
		if (!strcmp(check, ""))
		{
			cout << "Invalid file 1\n";
			return false;
		}
		if (check[0] == '<' and check[1] == '/')
		{
			char tag[TAG_LIMIT];
			lineIndex = 2;
			extractWord(check, tag, lineIndex, TAG_LIMIT, false, '>');
			if (!strcmp(tag, current->tag))
			{
				if (current->parent == nullptr)
				{
					rootIsClosed = true;
				}
				else
				{
					current = current->parent;
				}
			}
			else
			{
				cout << "Invalid file 2\n";
				return 0;
			}
		}
		else if (check[0] == '<')
		{
			Element child;
			status = child.readElementData(fileLine);
			if (status == -1)
			{
				cout << "Invalid file 3\n";
				return false;
			}
			else
			{
				current->addChild(child);
				if (status == 0)
				{
					current = current->getChildren()[current->getNumberOfChildren()-1];
				}
			}
		}
		else
		{
			char text[TEXT_LIMIT];
			while (fileLine[lineIndex] == ' ' or fileLine[lineIndex] == 9) lineIndex++;
			for (int i = 0; fileLine[lineIndex - 1] != '\0'; i++, lineIndex++)
			{
				text[i] = fileLine[lineIndex];
			}
			current->setText(text);
		}
	}
	f.close();
	return true;
}

//bool readFromFile(const char* filename, Element**& arr, int& size)
//{
//	ifstream f;
//	f.open(filename);
//	if (f.fail())
//	{
//		cout << "Error, file failed to open\n";
//		f.close();
//		return false;
//	}
//	char fileLine[INPUT_LIMIT];
//	f.getline(fileLine, INPUT_LIMIT);
//	Element newElement;
//	int status = newElement.readElementData(fileLine);
//	if (status == -1)
//	{
//		return false;
//	}
//	addNewElement(arr, newElement, size);
//	Element* current = arr[0];
//	bool rootIsClosed = (status == 1);
//	while (!rootIsClosed)
//	{
//		f.getline(fileLine, INPUT_LIMIT);
//		int lineIndex = 0;
//		char check[TAG_LIMIT+3];
//		extractWord(fileLine, check, lineIndex, TAG_LIMIT + 3, true);
//		if (!strcmp(check, ""))
//		{
//			cout << "Invalid file 1\n";
//			return false;
//		}
//		if (check[0] == '<' and check[1] == '/')
//		{
//			char tag[TAG_LIMIT];
//			lineIndex = 2;
//			extractWord(check, tag, lineIndex, TAG_LIMIT, false, '>');
//			if (!strcmp(tag, current->tag))
//			{
//				if (current->parent == nullptr)
//				{
//					rootIsClosed = true;
//				}
//				else
//				{
//					current = current->parent;
//				}
//			}
//			else
//			{
//				cout << "Invalid file 2\n";
//				return 0;
//			}
//		}
//		else if (check[0] == '<')
//		{
//			Element child;
//			status = child.readElementData(fileLine);
//			if (status == -1)
//			{
//				cout << "Invalid file 3\n";
//				return false;
//			}
//			else
//			{
//				addNewElement(arr, child, size);
//				current->addChild(*arr[size-1]);
//				if (status == 0)
//				{
//					current = arr[size - 1];
//				}
//			}
//		}
//		else
//		{
//			char text[TEXT_LIMIT];
//			while (fileLine[lineIndex] == ' ' or fileLine[lineIndex] == 9) lineIndex++;
//			for (int i = 0; fileLine[lineIndex - 1] != '\0'; i++, lineIndex++)
//			{
//				text[i] = fileLine[lineIndex];
//			}
//			current->setText(text);
//		}
//	}
//	f.close();
//	return true;
//}

bool Element::writeToFile(const char* filename)
{
	ofstream f;
	f.open(filename, ios::trunc);
	if (f.fail())
	{
		cout << "Error, couldn't open file " << filename << endl;
		return false;
	}
	printToFile(f);
	f.close();
	return true;
}

/*bool writeToFile(const char* filename, Element** arr, int size)
{
	ofstream f;
	f.open(filename, ios::trunc);
	if (f.fail())
	{
		cout << "Error, couldn't open file " << filename << endl;
		return false;
	}
	arr[0]->printToFile(f);
	f.close();
	return true;
}*/

Element* Element::findElementById(const char* id)
{
	if (!strcmp(this->id, id))
		return this;
	for (int i = 0; i < numberOfChildren; i++)
	{
		Element* search = children[i]->findElementById(id);
		if (search != nullptr)
			return search;
	}
	return nullptr;
}

/*Element* findElementById(Element** arr, int size, const char* id)
{
	for(int i = 0; i<size; i++)
		if (!strcmp(arr[i]->id, id))
		{
			return arr[i];
		}
	return nullptr;
}*/

void Element::assignUniqueIDs(ListOfIDs& list)
{
	if (!strcmp(id, "unknown"))
	{
		if (parent != nullptr)
		{
			setID(parent->id);
			bool found = false;
			for (int i = 0; i < parent->numberOfChildren and !found; i++)
			{
				if (*parent->children[i] == *this)
				{
					char suffix[2] = "0";
					suffix[0] = (i % 10) + '0';
					concatToID(suffix);
					found = true;
				}
			}
		}
		else
		{
			setID("0");
		}
	}
	if (!list.isTaken(id))
	{
		list.addID(id);
	}
	else 
	{
		makeIDUnique(list);
		list.addID(id);
	}
	for (int i = 0; i < numberOfChildren; i++)
		children[i]->assignUniqueIDs(list);
}