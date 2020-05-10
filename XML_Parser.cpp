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

void extractWord(char* source, char* destination, int& startIndex, int limit, bool keepStartIndex = false, char divider = ' ')
{
	int index = startIndex;
	while (source[index] == ' ' or source[index] == divider) index++;
	bool endOfWord = false;
	for (int i = 0; i < limit and !endOfWord; i++)
	{
		if (source[index] == divider or source[index] == '\0' or source[index] == ' ')
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

#include "Commands.hpp"
#include "Element.hpp"

int main()
{
	int size = 0;
	Element** arr = nullptr;

	char inputLine[INPUT_LIMIT];
	char command[COMMAND_LIMIT];
	char filename[FILENAME_LIMIT];
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
			if (readFromFile(filename, arr, size))
			{
				cout << "Successfully opened " << filename << endl;
				assignUniqueIDs(arr, size);
			}
		}
		else if (!strcmp(command, "print"))
		{
			if (size == 0)
				cout << "No currently stored data\n";
			else
			{
				arr[0]->print();
			}
		}
		else if (!strcmp(command, "close"))
		{
			if (size == 0)
			{
				cout << "No currently open file\n";
			}
			else
			{
				for (int i = 0; i < size; i++)
				{
					delete arr[i];
				}
				delete[] arr;
				arr = nullptr;
				size = 0;
				cout << "Successfully closed " << filename << endl;
			}
		}
		else if (!strcmp(command, "save"))
		{
			if (size == 0)
			{
				cout << "No currently open file\n";
			}
			else
			{
				if (writeToFile(filename, arr, size))
					cout << "Successfully saved " << filename << endl;
			}
		}
		else if (!strcmp(command, "saveas"))
		{
			if (size == 0)
			{
				cout << "No currently open file\n";
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
				if (writeToFile(newFilename, arr, size))
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
			Element* found = findElementById(arr, size, id);
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
			Element* found = findElementById(arr, size, id);
			if (found != nullptr)
			{
				//insert some cool code
			}
			else
			{
				cout << "There is no element with ID = " << id << endl;
			}
		}
	} while (strcmp(command, "exit"));
	cout << "Exiting the program...\n";
	for (int i = 0; i < size; i++)
	{
		delete arr[i];
	}
	delete[] arr;
}