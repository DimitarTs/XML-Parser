#define _CRT_SECURE_NO_WARNINGS //So I can use strcpy
#include <fstream>
#include <iostream>
using namespace std;

#include "Commands.hpp"
#include "Element.hpp"

void extractWord(char* source, char* destination, int& startIndex, int limit, bool keepStartIndex = false, char divider = ' ')
{
	int index = startIndex;
	while (source[index] == ' ') index++;
	bool endOfWord = false;
	for (int i = 0; i < limit and !endOfWord; i++)
	{
		if (source[index] == divider or source[index] == '\0')
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

int main()
{
	const int INPUT_LIMIT = 1024;
	char inputLine[INPUT_LIMIT];
	//cin.getline(inputLine, INPUT_LIMIT);
	char command[64];
	int index = 0;
	//extractWord(inputLine, command, index, 64);
	int size = 4;
	Element* arr = new Element[size];
	int rootIndex = 0;
	arr[2];
	arr[0].setTag("a");
	arr[3].setTag("d");
	arr[0].setID("1");
	arr[1].setID("1");
	arr[2].setID("1");
	arr[3].setID("1");
	arr[0].addAttribute("size", "2");
	arr[1].setText("test");
	arr[1].setTag("b");
	arr[0].addChild(arr[1]);
	arr[2].setTag("c");
	arr[2].setText("stuff");
	arr[2].addAttribute("size", "1");
	arr[2].addAttribute("color", "blue");
	arr[1].addChild(arr[2]);
	arr[1].addChild(arr[3]);
	arr[1].removeText();
	assignUniqueIDs(arr, 4);
	Element a;
	addNewElement(arr, a, size);
	arr[rootIndex].print();
	/*if (!strcmp(command, "print"))
	{
		arr[rootIndex].print();
	}*/
	delete[] arr;
}