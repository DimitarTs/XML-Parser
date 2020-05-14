class Attribute
{
	char* name;
	char* value;
	void copyFrom(const Attribute& other);
public:
	Attribute();
	Attribute(const Attribute& other);
	Attribute(const char*, const char*);
	~Attribute();
	Attribute& operator=(const Attribute& other);
	void setName(const char*);
	void setValue(const char*);
	const char* getName();
	const char* getValue();
	void print() const;
	void printToFile(ofstream&);
};
void Attribute::copyFrom(const Attribute& other)
{
	if (name != nullptr)
		delete[] name;
	int nameLen = strlen(other.name) + 1;
	name = new char[nameLen];
	for (int i = 0; i < nameLen; i++)
		name[i] = other.name[i];

	if (value != nullptr)
		delete[] value;
	int valueLen = strlen(other.value) + 1;
	value = new char[valueLen];
	for (int i = 0; i < valueLen; i++)
		value[i] = other.value[i];
}
Attribute::Attribute()
{
	name = new char[8];
	strcpy(name, "unknown");
	value = new char[8];
	strcpy(value, "unknown");
}
Attribute::Attribute(const Attribute& other)
{
	name = value = nullptr;
	copyFrom(other);
}
Attribute::Attribute(const char* name, const char* value)
{
	setName(name);
	setValue(value);
}
Attribute& Attribute::operator=(const Attribute& other)
{
	copyFrom(other);
	return *this;
}
Attribute::~Attribute()
{
	delete[] name;
	delete[] value;
}
void Attribute::setName(const char* name)
{
	if (this->name != nullptr)
		delete[] this->name;
	int nameLen = strlen(name) + 1;
	this->name = new char[nameLen];
	for (int i = 0; i < nameLen; i++)
		this->name[i] = name[i];
}
void Attribute::setValue(const char* value)
{
	if (this->value != nullptr)
		delete[] this->value;
	int valueLen = strlen(value) + 1;
	this->value = new char[valueLen];
	for (int i = 0; i < valueLen; i++)
		this->value[i] = value[i];
}
const char* Attribute::getName()
{
	return name;
}
const char* Attribute::getValue()
{
	return value;
}
void Attribute::print() const
{
	cout << name << "=" << '"' << value << '"';
}

void Attribute::printToFile(ofstream& f)
{
	f << name << "=" << '"' << value << '"';
}