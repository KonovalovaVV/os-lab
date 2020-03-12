#include <iostream>
#include <windows.h>
#include <fstream>
#include <string>

using namespace std;
const int tinyArray = 4, medArray = 20, bigArray = 100;

void printFile(char* nameOfFile, bool isBinary = false)
{
	cout << "\n\n" << nameOfFile << ":\n";
	ifstream fin;

	if (isBinary)
		fin.open(nameOfFile, ios::binary | ios::out);
	else
		fin.open(nameOfFile);

	while (!fin.eof())
	{
		string tmp;
		getline(fin, tmp);
		cout << tmp << endl;
	}
	fin.close();

	cout << "\n\n";
}
void startNewProcessAndWait(char* params, char* nameOfProcess)
{
	STARTUPINFOA sinf;
	ZeroMemory(&sinf, sizeof(STARTUPINFO));
	sinf.cb = sizeof(STARTUPINFO);

	PROCESS_INFORMATION pi;
	CreateProcessA(nameOfProcess, params, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &sinf, &pi);
	DWORD exit_code;
	if (FALSE == GetExitCodeProcess(pi.hProcess, &exit_code))
	{
		std::cerr << "GetExitCodeProcess() failure: " <<
			GetLastError() << "\n";
	}
	if (WaitForSingleObject(pi.hProcess, INFINITE) == WAIT_FAILED)
	{
		std::cerr << "GetExitCodeProcess() failure: " <<
			GetLastError() << "\n";
	}
}

int main()
{
	//setlocale(LC_ALL, "RU");

	char nameOfBinFile[medArray];
	cout << "Enter name of bin file\n";
	cin.getline(nameOfBinFile, medArray);

	char num[tinyArray];
	cout << "Enter num of records\n";
	cin.getline(num, tinyArray);

	char outString[bigArray];
	sprintf_s(outString, " %s %s", nameOfBinFile, num);

	startNewProcessAndWait(outString, (char*)"Creator.exe");
	printFile(nameOfBinFile, true);

	char nameOfReportFile[medArray];
	printf("\nEnter name of report file\n");
	cin.getline(nameOfReportFile, medArray);

	char grade[tinyArray];
	cout << "Enter salary \n";
	cin.getline(grade, tinyArray);

	sprintf_s(outString, " %s %s %s", nameOfBinFile, nameOfReportFile, grade);

	startNewProcessAndWait(outString, (char*)"Reporter.exe");
	printFile(nameOfReportFile, false);
}

