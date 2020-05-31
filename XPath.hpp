class Predicate
{
public:
	virtual bool isMetIn(const Element) const = 0;
	virtual Predicate* createNewPredicate() const = 0;
	virtual void print() const = 0;
	virtual ~Predicate() {}
};

class PredicateIndex : virtual public Predicate
{
	int index;
public:
	PredicateIndex();
	PredicateIndex(int index);

	Predicate* createNewPredicate() const;
	void setIndex(int index) 
	{
		if (index < 0)
			index = 0;
		this->index = index;
	}
	int getIndex() const;
	bool isMetIn(const Element element) const;
	void print() const;
	
};
PredicateIndex::PredicateIndex() : index(0) {}
PredicateIndex::PredicateIndex(int index) : index(index) {}
Predicate* PredicateIndex::createNewPredicate() const
{
	return new PredicateIndex;
}
int PredicateIndex::getIndex() const
{
	return index;
}
bool PredicateIndex::isMetIn(const Element element) const
{
	return ((index >= 0) && (element.getParent() != nullptr) && (element.getParent()->getNumberOfChildren() > index) && (*element.getParent()->getChildren()[index] == element));
}
void PredicateIndex::print() const
{
	cout << index;
}

class PredicateChild : virtual public Predicate
{
	char* tag;
	char* text;
public:
	PredicateChild();
	PredicateChild(const char* tag);
	PredicateChild(const char* tag, const char* text);
	PredicateChild(const PredicateChild& other);
	~PredicateChild();
	PredicateChild& operator=(const PredicateChild& other);

	Predicate* createNewPredicate() const;

	void setTag(const char* tag);
	void setText(const char* text);
	const char* getTag() const;
	const char* getText() const;

	bool isMetIn(const Element element) const;
	void print() const;
};
PredicateChild::PredicateChild() : text(nullptr)
{
	tag = new char[8];
	strcpy(tag, "unknown");
}
PredicateChild::PredicateChild(const char* tag) : text(nullptr)
{
	this->tag = new char[strlen(tag) + 1];
	strcpy(this->tag, tag);
}
PredicateChild::PredicateChild(const char* tag, const char* text)
{
	this->tag = new char[strlen(tag) + 1];
	strcpy(this->tag, tag);

	this->text = new char[strlen(text) + 1];
	strcpy(this->text, text);
}
PredicateChild::PredicateChild(const PredicateChild& other)
{
	this->tag = new char[strlen(other.tag) + 1];
	strcpy(this->tag, other.tag);

	this->text = new char[strlen(other.text) + 1];
	strcpy(this->text, other.text);
}
PredicateChild::~PredicateChild()
{
	if (tag != nullptr)
		delete[] tag;
	if (text != nullptr)
		delete[] text;
}
PredicateChild& PredicateChild::operator=(const PredicateChild& other)
{
	if (this == &other)
		return *this;

	if (tag != nullptr)
		delete[] tag;
	this->tag = new char[strlen(other.tag) + 1];
	strcpy(this->tag, other.tag);

	if (text != nullptr)
		delete[] text;
	this->text = new char[strlen(other.text) + 1];
	strcpy(this->text, other.text);

	return *this;
}

Predicate* PredicateChild::createNewPredicate() const
{
	return new PredicateChild(tag, text);
}

void PredicateChild::setTag(const char* tag)
{
	if (this->tag != nullptr)
		delete[] this->tag;
	this->tag = new char[strlen(tag) + 1];
	strcpy(this->tag, tag);
}
void PredicateChild::setText(const char* text)
{
	if (this->text != nullptr)
		delete[] this->text;
	this->text = new char[strlen(text) + 1];
	strcpy(this->text, text);
}
const char* PredicateChild::getTag() const
{
	return tag;
}
const char* PredicateChild::getText() const
{
	return text;
}

bool PredicateChild::isMetIn(const Element element) const
{
	int numberOfChildren = element.getNumberOfChildren();
	for (int i = 0; i < numberOfChildren; i++)
	{
		if ((!strcmp(element.getChildren()[i]->getTag(), tag)) && (!strcmp(element.getChildren()[i]->getText(), text)))
			return true;
	}
	return false;
}
void PredicateChild::print() const
{
	if (tag != nullptr)
	{
		cout << tag;
		if (text != nullptr)
			cout << " " << text;
		else cout << " text is nullptr";
	}
	else cout << "tag is nullptr";
}

class PredicateAttribute : virtual public Predicate, public Attribute
{
public:
	PredicateAttribute();
	PredicateAttribute(const PredicateAttribute& other);
	PredicateAttribute(const char* name);
	PredicateAttribute(const char* name, const char* value);
	PredicateAttribute& operator=(const PredicateAttribute& other);
	~PredicateAttribute();

	Predicate* createNewPredicate() const;

	bool isMetIn(const Element element) const;
	void print() const;
};
PredicateAttribute::PredicateAttribute() : Attribute() {}
PredicateAttribute::PredicateAttribute(const PredicateAttribute& other) : Attribute(other) {}
PredicateAttribute::PredicateAttribute(const char* name) : Attribute(name, "unknown") {}
PredicateAttribute::PredicateAttribute(const char* name, const char* value) : Attribute(name, value) {}
PredicateAttribute& PredicateAttribute::operator=(const PredicateAttribute& other)
{
	Attribute::operator=(other);
	return *this;
}
PredicateAttribute::~PredicateAttribute() {}
Predicate* PredicateAttribute::createNewPredicate() const
{
	return new PredicateAttribute(*this);
}
bool PredicateAttribute::isMetIn(const Element element) const
{
	if (strcmp(getName(), "id"))
	{
		const char* value = element.getAttributeValue(getName());
		if (value == nullptr)
			return false;

		if (!strcmp(getValue(), "unknown"))
			return true;

		if (!strcmp(getValue(), value))
			return true;

		return false;
	}
	else
	{
		if (!strcmp(getValue(), "unknown"))
			return true;
		else
			return !strcmp(getValue(), element.getID());
	}
}
void PredicateAttribute::print() const
{
	cout << getName() << " " << getValue();
}

class XPath
{
	char* tag;
	Predicate* criteria;
	XPath* next;
public:
	XPath();
	XPath(const XPath& other);
	~XPath();
	XPath& operator=(const XPath& other);

	void addToTag(char c);
	void popTag();
	void interprete(const char* str);
	void execute(Element root);
	void print();
};
XPath::XPath() : tag(nullptr), criteria(nullptr), next(nullptr) {}
XPath::XPath(const XPath& other)
{
	if (other.tag == nullptr)
		tag = nullptr;
	else
	{
		tag = new char[strlen(other.tag) + 1];
		strcpy(tag, other.tag);
	}

	if (other.criteria == nullptr)
		criteria = nullptr;
	else
		criteria = other.criteria->createNewPredicate();

	if (other.next == nullptr)
		next = nullptr;
	else
		next = new XPath(*other.next);
}
XPath::~XPath()
{
	if (tag != nullptr)
		delete[] tag;
	if (criteria != nullptr)
		delete criteria;
	if (next != nullptr)
		delete next;
}
XPath& XPath::operator=(const XPath& other)
{
	if (this == &other)
		return *this;

	if (tag != nullptr)
		delete[] tag;
	if (other.tag == nullptr)
		tag = nullptr;
	else
	{
		tag = new char[strlen(other.tag) + 1];
		strcpy(tag, other.tag);
	}

	if (criteria != nullptr)
		delete criteria;
	if (other.criteria == nullptr)
		criteria = nullptr;
	else
		criteria = other.criteria->createNewPredicate();

	if (next != nullptr)
		delete next;
	if (other.next == nullptr)
		next = nullptr;
	else
		next = new XPath(*other.next);

	return *this;
}
void XPath::addToTag(char c)
{
	if (tag == nullptr)
	{
		tag = new char[2];
		tag[0] = c;
		tag[1] = '\0';
		return;
	}

	char* newTag = new char[strlen(tag) + 2];
	int i;
	for (i = 0; tag[i] != '\0'; i++)
	{
		newTag[i] = tag[i];
	}
	newTag[i] = c;
	newTag[i + 1] = '\0';

	delete[] tag;
	tag = newTag;
}
void XPath::popTag()
{
	if (tag == nullptr)
		return;
	if (strlen(tag) == 1)
	{
		delete[] tag;
		tag = nullptr;
		return;
	}

	char* newTag = new char[strlen(tag)];
	int i;
	for (i = 0; tag[i + 1] != '\0'; i++)
	{
		newTag[i] = tag[i];
	}
	newTag[i] = '\0';
	delete[] tag;
	tag = newTag;
}
void XPath::interprete(const char* str)
{
	XPath* current = this;
	int index = 0;
	if (str[index] == '/') index++;
	for (int i = index; str[i] != '\0'; i++)
	{
		if (str[i] == '/')
		{
			current->next = new XPath;
			current = current->next;
		}
		else if (str[i] == '[')
		{
			i++;
			if (str[i] >= '0' and str[i] <= '9')
			{
				current->criteria = new PredicateIndex(extractNumber(str, i));
				if (str[i] != ']')
				{
					cout << "\n\nUNEXPECTED CHARACTER " << i << str[i] << " !!!\n\n\n";
				}
			}
			else if (str[i] == '@')
			{
				i++;
				char key[KEY_LIMIT];
				int keyindex = 0;
				while (str[i] != '=' && str[i] != ']')
					key[keyindex++] = str[i++];
				key[keyindex] = '\0';
				if (str[i] == ']')
				{
					current->criteria = new PredicateAttribute(key);
				}
				else
				{
					i++;
					if (str[i] != '"')
					{
						cout << "\n\nUNEXPECTED CHARACTER " << i << str[i] << " !!!\n\n\n";
					}
					else
					{
						i++;
						char value[VALUE_LIMIT];
						int valueindex = 0;
						while (str[i] != '"')
							value[valueindex++] = str[i++];
						value[valueindex] = '\0';
						current->criteria = new PredicateAttribute(key, value);
						i++;
						if (str[i] != ']')
						{
							cout << "\n\nUNEXPECTED CHARACTER " << i << str[i] << " !!!\n\n\n";
						}
					}
				}
			}
			else
			{
				char tag[TAG_LIMIT];
				int tagindex = 0;
				while (str[i] != '=' && str[i] != ']')
					tag[tagindex++] = str[i++];
				tag[tagindex] = '\0';
				if (str[i] == ']')
				{
					current->criteria = new PredicateChild(tag);
					continue;
				}
				else
				{
					i++;
					if (str[i] != '"')
					{
						cout << "\n\nUNEXPECTED CHARACTER " << i << str[i] << " !!!\n\n\n";
					}
					else
					{
						i++;
						char text[TEXT_LIMIT];
						int textindex = 0;
						while (str[i] != '"')
						{
							text[textindex++] = str[i++];
						}
						text[textindex] = '\0';
						current->criteria = new PredicateChild(tag, text);
						i++;
						if (str[i] != ']')
						{
							cout << "\n\nUNEXPECTED CHARACTER " << i << str[i] << " !!!\n\n\n";
						}
					}
				}
			}
		}
		else
		{
			current->addToTag(str[i]);
		}
	}
}
void XPath::execute(Element root)
{
	int numberOfChildren = root.getNumberOfChildren();
	/*cout << "looking at " << root.getTag() << " ";
	if (criteria == nullptr)
	{
		cout << " with no criteria\n";
	}
	else
	{
		criteria->print();
		cout << endl;
	}*/
	for (int i = 0; i < numberOfChildren; i++)
	{
		if (!strcmp(tag, root.getChildren()[i]->getTag()))
		{
			if (criteria == nullptr or criteria->isMetIn(*root.getChildren()[i]))
			{
				if (next == nullptr)
					cout << root.getChildren()[i]->getText() << endl;
				else
				{
					next->execute(*root.getChildren()[i]);
				}
			}
		}
	}
}
void XPath::print()
{
	if (tag == nullptr)
		return;
	cout << tag << " ";
	if (criteria != nullptr)
		criteria->print();
	cout << endl;
	if (next != nullptr)
		next->print();
}