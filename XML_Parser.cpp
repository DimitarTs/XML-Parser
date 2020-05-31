#define _CRT_SECURE_NO_WARNINGS //So I can use strcpy
#include <fstream>
#include <iostream>
using namespace std;

const int INPUT_LIMIT = 1024;
const int FILENAME_LIMIT = 256;
const int COMMAND_LIMIT = 64;
const int ID_LIMIT = 64;
const int KEY_LIMIT = 64;
const int VALUE_LIMIT = 64;
const int TEXT_LIMIT = 256;
const int TAG_LIMIT = 64;
const int XPATH_LIMIT = 256;

void extractWord(const char* source, char* destination, int& startIndex, int limit, bool keepStartIndex = false, char divider = ' ')
{
	int index = startIndex;
	while (source[index] == ' ' or source[index] == 9 or source[index] == divider) index++;
	bool endOfWord = false;
	for (int i = 0; i < limit and !endOfWord; i++)
	{
		if (source[index] == divider or source[index] == '\0' or source[index] == ' ' or source[index] == 9)
		{
			endOfWord = true;
			destination[i] = '\0';
		}
		else
		{
			destination[i] = source[index++];
		}

		if (i == limit - 1 && destination[i] != '\0')
			destination[0] = '\0';
	}
	if (!keepStartIndex)
		startIndex = index;
	return;
}

int extractNumber(const char* source, int& startIndex, bool keepStartIndex = false)
{
	int result = 0;
	int index = startIndex;
	while (source[index] == ' ') index++;
	while (source[index] >= '0' and source[index] <= '9')
		result = 10 * result + source[index++] - '0';
	if (!keepStartIndex)
		startIndex = index;
	return result;
}

#include "Commands.hpp"
#include "Element.hpp"
#include "XPath.hpp"

int main()
{
	Element* root = nullptr;
	char inputLine[INPUT_LIMIT];
	char command[COMMAND_LIMIT];
	char filename[FILENAME_LIMIT];
	bool openFile = false;
	do 
	{
		cout << "> ";
		cin.getline(inputLine, INPUT_LIMIT);
		int index = 0;
		extractWord(inputLine, command, index, COMMAND_LIMIT);
		if (!strcmp(command, "help"))
		{
			help();
		}
		else if (!strcmp(command, "open"))
		{
			if (root != nullptr)
			{
				cout << "There is a currently stored data, please use 'close' before opening a file\n";
				continue;
			}
			while (inputLine[index] == ' ') index++;
			if (inputLine[index] == '"')
				index++;
			for (int i = 0; inputLine[index] != '\0' and inputLine[index] != '"' and i < FILENAME_LIMIT - 1; i++, index++)
			{
				filename[i] = inputLine[index];
				if (inputLine[index + 1] == '\0' or inputLine[index + 1] == '"' or i == FILENAME_LIMIT - 2)
				{
					filename[i + 1] = '\0';
				}
			}
			root = new Element;
			if (root->readFromFile(filename))
			{
				cout << "Successfully opened " << filename << endl;
				openFile = true;
				ListOfIDs list;
				root->assignUniqueIDs(list);
			}
			else
			{
				delete root;
				root = nullptr;
				cout << "Failed to open " << filename << endl;
			}
		}
		else if (!strcmp(command, "print"))
		{
			if (root == nullptr)
				cout << "No currently stored data\n";
			else
			{
				root->print();
			}
		}
		else if (!strcmp(command, "close"))
		{
			if (root == nullptr)
			{
				cout << "No currently stored data\n";
			}
			else
			{
				delete root;
				root = nullptr;
				if (openFile)
				{
					cout << "Successfully closed " << filename << endl;
					openFile = false;
				}
				else cout << "Successfully deleted all stored data\n";
			}
		}
		else if (!strcmp(command, "save"))
		{
			if (!openFile)
			{
				cout << "No currently open file\n";
			}
			else
			{
				if (root->writeToFile(filename))
					cout << "Successfully saved " << filename << endl;
			}
		}
		else if (!strcmp(command, "saveas"))
		{
			if (root == nullptr)
			{
				cout << "No currently stored data\n";
			}
			else
			{
				char newFilename[FILENAME_LIMIT];
				while (inputLine[index] == ' ') index++;
				if (inputLine[index] == '"')
					index++;
				for (int i = 0; inputLine[index] != '\0' and inputLine[index] != '"' and i < FILENAME_LIMIT - 1; i++, index++)
				{
					newFilename[i] = inputLine[index];
					if (inputLine[index + 1] == '\0' or inputLine[index + 1] == '"' or i == FILENAME_LIMIT - 2)
					{
						newFilename[i + 1] = '\0';
					}
				}
				if (root->writeToFile(newFilename))
					cout << "Successfully saved " << newFilename << endl;
				else
					cout << "Error, file wasn't saved\n";
			}
		}
		else if (!strcmp(command, "select"))
		{
			char id[ID_LIMIT];
			extractWord(inputLine, id, index, ID_LIMIT);
			char key[KEY_LIMIT];
			extractWord(inputLine, key, index, KEY_LIMIT);
			Element* found = root->findElementById(id);
			if (found != nullptr)
			{
				const char* value = found->getAttributeValue(key);
				if (value != nullptr)
				{
					cout << key << "=";
					cout << '"' << value << '"' << endl;
				}
				else cout << id << " has no such attribute\n";
			}
			else
			{
				cout << "There is no element with ID = " << id << endl;
			}
		}
		else if (!strcmp(command, "set"))
		{
			char id[ID_LIMIT];
			extractWord(inputLine, id, index, ID_LIMIT);
			char key[KEY_LIMIT];
			extractWord(inputLine, key, index, KEY_LIMIT);
			char value[VALUE_LIMIT];
			extractWord(inputLine, value, index, VALUE_LIMIT);
			Element* found = root->findElementById(id);
			if (found != nullptr)
			{
				if (!found->setAttributeValue(key, value))
				{
					if (!strcmp(key, "tag"))
						found->setTag(value);
					else if (!strcmp(key, "id"))
					{
						found->setID(value);
						ListOfIDs list;
						root->assignUniqueIDs(list);
					}
					else
						found->addAttribute(key, value);
				}
				cout << "Successfully set " << key << " of " << id << " to " << value << endl;
			}
			else
			{
				cout << "There is no element with ID = " << id << endl;
			}
		}
		else if (!strcmp(command, "children"))
		{
			char id[ID_LIMIT];
			extractWord(inputLine, id, index, ID_LIMIT);
			Element* found = root->findElementById(id);
			if (found != nullptr)
			{
				int numberOfChildren = found->getNumberOfChildren();
				Element**const children = found->getChildren();
				const Attribute* attributes;
				for (int i = 0; i < numberOfChildren; i++)
				{
					int numberOfAttributes = children[i]->getNumberOfAttributes();
					attributes = children[i]->getAttributes();
					cout << children[i]->getTag() << ":";
					for (int i = 0; i < numberOfAttributes; i++)
					{
						cout << " ";
						attributes[i].print();
					}
					cout << endl;
				}
			}
			else
			{
				cout << "There is no element with ID = " << id << endl;
			}
		}
		else if (!strcmp(command, "child"))
		{
			char id[ID_LIMIT];
			extractWord(inputLine, id, index, ID_LIMIT);
			Element* found = root->findElementById(id);
			if (found != nullptr)
			{
				int n = extractNumber(inputLine, index);
				Element** const children = found->getChildren();
				if (n > 0 and n <= found->getNumberOfChildren())
				{
					children[n - 1]->print();
				}
				else
				{
					cout << "Child number can range from 1 to " << found->getNumberOfChildren() << endl;
				}
			}
			else
			{
				cout << "There is no element with ID = " << id << endl;
			}
		}
		else if (!strcmp(command, "text"))
		{
			char id[ID_LIMIT];
			extractWord(inputLine, id, index, ID_LIMIT);
			Element* found = root->findElementById(id);
			if (found != nullptr)
			{
				char text[TEXT_LIMIT];
				while (inputLine[index] == ' ') index++;
				int textIndex = 0;
				while (inputLine[index - 1] != '\0' and textIndex < TEXT_LIMIT)
					text[textIndex++] = inputLine[index++];
				text[TEXT_LIMIT - 1] = '\0';
				if (!strcmp(text, ""))
				{
					if (found->getText() != nullptr)
						cout << found->getText() << endl;
					else
						cout << found->getTag() << " has no text\n";
				}
				else
				{
					found->setText(text);
					cout << "Successfully set the text of " << found->getTag() << endl;
				}
			}
			else
			{
				cout << "There is no element with ID = " << id << endl;
			}
		}
		else if (!strcmp(command, "removetext"))
		{
			char id[ID_LIMIT];
			extractWord(inputLine, id, index, ID_LIMIT);
			Element* found = root->findElementById(id);
			if (found != nullptr)
			{
				found->removeText();
				cout << "Successfully removed the text of " << found->getTag() << endl;
			}
			else
			{
				cout << "There is no element with ID = " << id << endl;
			}
		}
		else if (!strcmp(command, "delete"))
		{
			char id[ID_LIMIT];
			extractWord(inputLine, id, index, ID_LIMIT);
			char key[KEY_LIMIT];
			extractWord(inputLine, key, index, KEY_LIMIT);
			Element* found = root->findElementById(id);
			if (found != nullptr)
			{
				if (strcmp(key, "")) 
				{
					if (found->removeAttribute(key))
						cout << "Successfully removed " << key << " from " << id << endl;
					else cout << id << " doesn't contain the attribute " << key << endl;
				}
				else
				{
					if (found->getParent() != nullptr)
					{
						found->getParent()->removeChild(*found);
						cout << "Successfully removed element and all of it's decendants\n";
					}
					else
						cout << "Cannot delete root element\n";
				}
			}
			else
			{
				cout << "There is no element with ID = " << id << endl;
			}
		}
		else if (!strcmp(command, "newchild"))
		{
			char id[ID_LIMIT];
			extractWord(inputLine, id, index, ID_LIMIT);
			Element* found = root->findElementById(id);
			if (found != nullptr)
			{
				Element empty;
				found->addChild(empty);
				ListOfIDs list;
				root->assignUniqueIDs(list);
				cout << "Successfully added a new child to " << found->getTag() << endl;
			}
			else
			{
				cout << "There is no element with ID = " << id << endl;
			}
		}
		else if (!strcmp(command, "newroot"))
		{
			if (root == nullptr) 
			{
				root = new Element;
				ListOfIDs list;
				root->assignUniqueIDs(list);
				cout << "Successfully created new root\n";
			}
			else
			{
				cout << "There already is a root. Close file or delete all data to create new\n";
			}
		}
		else if (!strcmp(command, "xpath"))
		{
			char id[ID_LIMIT];
			extractWord(inputLine, id, index, ID_LIMIT);
			Element* found = root->findElementById(id);
			if (found != nullptr)
			{
				char xpathInStr[XPATH_LIMIT];
				while (inputLine[index] == ' ')
					index++;
				int xpathindex = 0;
				while (inputLine[index] != '\0' and xpathindex < XPATH_LIMIT - 1)
				{
					xpathInStr[xpathindex++] = inputLine[index++];
				}
				xpathInStr[xpathindex] = '\0';
				XPath xpath;
				xpath.interprete(xpathInStr);
				xpath.execute(*found);
			}
			else
			{
				cout << "There is no element with ID = " << id << endl;
			}
		}
		else if (strcmp(command, "exit"))
		{
			cout << "Unrecognized command, use 'help' for list of supported commands\n";
		}
	} while (strcmp(command, "exit"));
	cout << "Exiting the program...\n";
	delete root;
}