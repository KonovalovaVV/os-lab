#include <iostream>
#include <string.h>
#include <fstream>
using namespace std;

struct employee
{
	int num;
	char name[10];
	double hours;
};

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		cout << "ERROR: wrong number of	parameters\n";
		return 1;
	}
	ofstream binFile(argv[1], ios::binary | ios::out);
	int numberOfEmployes = atoi(argv[2]);

	employee tmp;
	for (int i = 0; i < numberOfEmployes; i++)
	{
		cout << "Enter num:\n";
		cin >> tmp.num;
		cin.get();

		cout << "Enter name:\n";
		cin.getline(tmp.name, 10);

		cout << "Enter hours:\n";
		cin >> tmp.hours;

		binFile.write((char *)&tmp, sizeof(employee));
	}

	binFile.close();
}
