#include <iostream>
#include <string.h>
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include  <stdio.h>

struct employee
{
	int num;
	char name[10];
	double hours;
};

int main(int argc, char** argv)
{
	setlocale(LC_ALL, "RU");
	if (argc != 4)
	{
		std::cout << "ERROR: wrong number of	parameters\n";
		return 1;
	}

	double salary = atof(argv[3]);

	FILE* binFile = NULL;
	FILE* reportFile = NULL;

	if(fopen_s(&binFile, argv[1], "r") != NULL)
		return GetLastError();

	if(fopen_s(&reportFile, argv[2], "w") != NULL)
		return GetLastError();

	if (binFile == NULL)
	{
		std::cout << "Bin file is not found\n";
		fclose(reportFile);
		return 1;
	}

	char outString[100];
	employee tmp;

	if(sprintf_s(outString, "Отчет по файлу %s\nНомер Имя Часы Зарплата\n",argv[1]) == -1)
		return GetLastError();

	fwrite(outString, strlen(outString), 1, reportFile);

	while (fread(&tmp, sizeof(employee), 1, binFile))
	{
		sprintf_s(outString, "%d %s %0.2f %0.2f\n", 
				  tmp.num, tmp.name, tmp.hours, tmp.hours*salary);
		fwrite(outString, strlen(outString), 1, reportFile);
	}

	fclose(reportFile);
	fclose(binFile);

}