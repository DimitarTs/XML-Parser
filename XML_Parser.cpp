#define _CRT_SECURE_NO_WARNINGS
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
	/*char inputLine[INPUT_LIMIT];
	cin.getline(inputLine, INPUT_LIMIT);
	char command[64];
	int index = 0;
	extractWord(inputLine, command, index, 64, 0, '=');
	cout << command << endl;
	index++;
	extractWord(inputLine, command, index, 64);
	cout << command << endl;*/
	Element a, b, c;
	a.setTag("a");
	a.addAttribute("size", "2");
	b.setText("test");
	b.setTag("b");
	a.addChild(b);
	c.setTag("c");
	c.setText("stuff");
	c.addAttribute("size", "1");
	c.addAttribute("color", "blue");
	b.addChild(c);
	b.removeText();
	a.print(0);
}