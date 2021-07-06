#include "pch.h"
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include "../server/employee.h"
#include "../server/constants.h"

HANDLE connect_pipe(char* pipename)
{
	if (!WaitNamedPipeA(pipename, NMPWAIT_WAIT_FOREVER))
	{
		std::cout << "Error with waiting for pipe: " << GetLastError() << std::endl;
		return NULL;
	}

	HANDLE hPipe = CreateFileA(
		pipename,
		GENERIC_READ |
		GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);

	if (hPipe == INVALID_HANDLE_VALUE)
	{
		std::cout << "Error with connecting pipe: " << GetLastError() << std::endl;
		return NULL;
	}
	return hPipe;
}

int send_message(HANDLE hPipe, char* message, int len)
{
	if (!WriteFile(hPipe, message, len, NULL, NULL))
	{
		std::cout << "Error with sending message " << GetLastError() << std::endl;
		return -1;
	}
	return 1;
}

void send_new_record(employee* empl, HANDLE hPipe)
{
	std::cout << "Enter employee id: ";
	std::cin >> empl->num;

	std::cout << "Enter employee name: ";
	std::cin.getline(empl->name, EMPLOYEE_NAME_LEN + 1);

	std::cout << "Enter count of hours worked: ";
	std::cin >> empl->hours;

	if (!WriteFile(hPipe, &empl, sizeof(struct Employee), NULL, NULL))
	{
		printf("WriteFile failed, %d\n", GetLastError());
		return;
	}
}

int main(int argc, char* argv[]) 
{
	HANDLE hPipe;
	char message[MESSAGE_SIZE];

	char pipename[80];

	sprintf_s(pipename, PIPENAME, atoi(argv[1]));

	hPipe = connect_pipe(pipename);

	employee tp;
	while(true)
	{
		char choice;

		std::cout << "Enter 'w' to modify record, 'r' to read record, 'q' to quit: ";
		std::cin >> choice;

		if (choice == 'q') 
		{
			send_message(hPipe, (char*)"q", 2);
			break;
		}

		int numOfRecord;

		std::cout << "Enter number of record: ";
		std::cin >> numOfRecord;

		if (choice == 'w')
		{
			sprintf_s(message, "WRITE %d", numOfRecord);
		}
		else if (choice == 'r')
		{
			sprintf_s(message, "READ %d", numOfRecord);
		}

		send_message(hPipe, message, MESSAGE_SIZE * sizeof(char));

		ReadFile(hPipe, &tp, sizeof(employee), NULL, NULL);
		printf("Employee %d: \n\t%d, \n\t%s, \n\t%lf\n", numOfRecord, tp.num, tp.name, tp.hours);

		if (choice == 'w')
		{
			send_new_record(&tp, hPipe);
			sprintf_s(message, "RELEASE WRITE");
		}
		else if (choice == 'r')
		{
			sprintf_s(message, "q");
		}

		printf("Enter 'e' to end: ");
		getchar();
		getchar();

		send_message(hPipe, message, MESSAGE_SIZE * sizeof(char));
	}

	CloseHandle(hPipe);
	return 0;
}
