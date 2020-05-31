void help()
{
	// Base commands:
	cout << "\nThe following commands are supported:\n";
	cout << "open <file>             opens <file>\n";
	cout << "close                   closes currently opened file\n";
	cout << "save                    saves the currently opened file\n";
	cout << "saveas <file>           saves the currently opened file in <file>\n";
	cout << "help                    lists this information\n";
	cout << "exit                    exits the program\n";
	cout << "print                   prints currently opened data\n";
	cout << "select <id> <key>       prints the value of attribute <key> of element with id = <id>\n";
	cout << "set <id> <key> <value>  sets the value of attribute <key> of element with id = <id> to <value>\n";
	cout << "children <id>           lists all attributes of all direct children of element with id = <id>\n";
	cout << "child <id> <n>          outputs data of the n-th child of element with id = <id>\n";
	cout << "text <id>               outputs the text of element with id = <id>\n";
	cout << "text <id> <text>        sets the text of element with id = <id> to <text>\n";
	cout << "removetext <id>         removes the text of element with id = <id>\n";
	cout << "delete <id>             Deletes element with id = <id> and all of its decendants\n";
	cout << "delete <id> <key>       deletes attribute <key> of element with id = <id>\n";
	cout << "newchild <id>           creates a new, blank child of element with id = <id>\n";
	cout << "newroot                 creates a new, blank root if there is no current root\n";
	cout << "xpath <id> <XPath>      execute <XPath> starting from element with id = <id>\n\n";
}