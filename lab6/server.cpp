#include "constants.h"

#include <stdio.h>
#include <iostream>

int create_file_by_input_name(HANDLE* file);
int fill_file(HANDLE file, int& records_num);
int print_file(HANDLE hFile, int num);
int map_file(HANDLE file, employee** filedata);
int connect_clients(employee** file_data, block_info* bi);
void fill_block_info(int records_num, block_info* bi);

DWORD WINAPI serve_client(LPVOID);

int main() 
{
	HANDLE hFile;
	if(create_file_by_input_name(&hFile) != 0)
		return -1;

	int records_num;
	fill_file(hFile, records_num);

	print_file(hFile, records_num);

	employee* filedata;
	map_file(hFile, &filedata);

	block_info bi;
	fill_block_info(records_num, &bi);

	connect_clients(&filedata, &bi);

	UnmapViewOfFile(filedata);
	print_file(hFile, records_num);

	CloseHandle(hFile);

	return 0;
}

void fill_block_info(int records_num, block_info* bi)
{
	std::vector<int> block_counts(records_num);
	std::vector<CRITICAL_SECTION> cs_access_to_file(records_num);
	std::vector<CRITICAL_SECTION> cs_access_to_write(records_num);
	std::vector<HANDLE> mutex_access_to_file(records_num);

	for (int i = 0; i < records_num; i++)
	{
		InitializeCriticalSection(&cs_access_to_file[i]);
		InitializeCriticalSection(&cs_access_to_write[i]);

		mutex_access_to_file[i] = CreateMutex(NULL, FALSE, NULL);
		if (mutex_access_to_file[i] == NULL)
		{
			std::cout << "Error with CreateMutex: " << GetLastError() << std::endl;
			return;
		}
	}

	bi->block_counts = block_counts;
	bi->cs_access_to_file = cs_access_to_file;
	bi->cs_access_to_write = cs_access_to_write;
	bi->mutex_access_to_file = mutex_access_to_file;
}

int create_file_by_input_name(HANDLE* file)
{
	char fileName[FILENAME_LEN];
	std::cout << "Enter a file name: ";
	std::cin.getline(fileName, FILENAME_LEN);

	*file = CreateFileA(fileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); 

	if (*file == INVALID_HANDLE_VALUE)
	{
		std::cout << "CreateFile failed, " << GetLastError() << std::endl;
		return -1;
	}
	return 0;
}

int fill_file(HANDLE file, int& records_num)
{
	std::cout << "Enter number of records: ";
	std::cin >> records_num;

	for (int i = 0; i < records_num; i++) 
	{
		employee empl;

		std::cout << "Enter id of employee " << i + 1 << ": ";
		std::cin >> empl.num;
		getchar();

		std::cout << "Enter employee name: ";
		std::cin.get(empl.name, EMPLOYEE_NAME_LEN);

		std::cout << "Enter count of hours worked of employee: ";
		std::cin >> empl.hours;

		WriteFile(file, &empl, sizeof(employee), NULL, NULL);
	}
	return 0;
}

int print_file(HANDLE hFile, int num) 
{
	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

	employee empl;
	std::cout << "\nFile data:\n";

	for (int i = 0; i < num; i++) 
	{
		ReadFile(hFile, &empl, sizeof(employee), NULL, NULL);
		printf("Employee %d: \n\tID:%d, \n\tName: %s, \n\tHours: %lf\n", i, empl.num, empl.name, empl.hours);
	}
	return 0;
}

int map_file(HANDLE file, employee** file_data)
{
	DWORD fileSize = GetFileSize(file, NULL);

	HANDLE hMapping = CreateFileMapping(file, NULL, PAGE_READWRITE, 0, 0, NULL);
	if (hMapping == NULL)
	{
		std::cout << "Eror with CreateFileMapping: " << GetLastError() << std::endl;
		return -1;
	}

	*file_data = (employee*)MapViewOfFile(hMapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, fileSize);

	if (*file_data == NULL)
	{
		std::cout << "Error with MapViewOfFile: " << GetLastError() << std::endl;
		return -1;
	}
	CloseHandle(hMapping);
	return 0;
}

HANDLE create_pipe(char* pipename)
{
	HANDLE hPipe = CreateNamedPipeA(
		pipename,
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE |
		PIPE_READMODE_MESSAGE |
		PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		0,
		0,
		0,
		NULL
	);
	if (hPipe == INVALID_HANDLE_VALUE)
	{
		std::cout << "Error with CreateNamedPipeA: " << GetLastError() << std::endl;
		return NULL;
	}
	return hPipe;
}

HANDLE create_thread(thread_data* td)
{
	HANDLE thr = CreateThread(
		NULL,
		0,
		serve_client,
		(LPVOID)td,
		0,
		NULL
	);
	if (thr == NULL)
	{
		std::cout << "CreateThread failed, " << GetLastError() << std::endl;
		return NULL;
	}
	return thr;
}

int connect_clients(employee** file_data, block_info* bi)
{
	int num;
	std::cout << "Enter number of clients: ";
	std::cin >> num;

	std::vector<HANDLE> pipes_handle(num, INVALID_HANDLE_VALUE);
	std::vector<HANDLE> threads(num, INVALID_HANDLE_VALUE);

	for (int i = 0; i < num; i++)
	{
		char pipename[80];
		sprintf_s(pipename, PIPENAME, i + 1);
		pipes_handle[i] = create_pipe(pipename);

		thread_data td;
		td.hPipe = pipes_handle[i];
		td.file_data = *file_data;
		td.bl_info = bi;

		threads[i] = create_thread(&td);
	}
	WaitForMultipleObjects(threads.size(), threads.data(), TRUE, INFINITE);
	return 0;
}

int WINAPI read_option(thread_data* td, char* message, int num_of_record)
{
	EnterCriticalSection(&td->bl_info->cs_access_to_file[num_of_record]);

	td->bl_info->block_counts[num_of_record]++;
	if (td->bl_info->block_counts[num_of_record] == 1)
		WaitForSingleObject(td->bl_info->mutex_access_to_file[num_of_record], INFINITE);

	LeaveCriticalSection(&td->bl_info->cs_access_to_file[num_of_record]);


	if (!WriteFile(td->hPipe, (td->file_data) + num_of_record, sizeof(employee), NULL, NULL)) 
	{
		std::cout << "Error with sending data: " << GetLastError() << std::endl;
		return -1;
	}

	if (!ReadFile(td->hPipe, message, MESSAGE_SIZE * sizeof(char), NULL, NULL)) 
	{
		std::cout << "Error with getting data: " << GetLastError() << std::endl;
		return -1;
	}

	EnterCriticalSection(&td->bl_info->cs_access_to_file[num_of_record]);

	td->bl_info->block_counts[num_of_record]--;
	if (td->bl_info->block_counts[num_of_record] == 0)
		ReleaseMutex(td->bl_info->mutex_access_to_file[num_of_record]);

	LeaveCriticalSection(&td->bl_info->cs_access_to_file[num_of_record]);
	return 0;
}

int WINAPI write_option(thread_data* td, int num_of_record, char* message)
{
	EnterCriticalSection(&td->bl_info->cs_access_to_write[num_of_record]);
	WaitForSingleObject(td->bl_info->mutex_access_to_file[num_of_record], INFINITE);

	if (!WriteFile(td->hPipe, (td->file_data) + num_of_record, sizeof(employee), NULL, NULL)) 
	{
		std::cout << "Error with sending data: " << GetLastError() << std::endl;
		return -1;
	}

	if (!ReadFile(td->hPipe, (td->file_data) + num_of_record, sizeof(employee), NULL, NULL)) 
	{
		std::cout << "Error with getting new data: " << GetLastError() << std::endl;
		return -1;
	}

	if (!ReadFile(td->hPipe, message, MESSAGE_SIZE * sizeof(char), NULL, NULL))
	{
		std::cout << "Error with getting finish data: " << GetLastError() << std::endl;
		return -1;
	}

	ReleaseMutex(td->bl_info->mutex_access_to_file[num_of_record]);

	LeaveCriticalSection(&td->bl_info->cs_access_to_write[num_of_record]);
	return 0;
}

DWORD WINAPI serve_client(LPVOID lpvParam) 
{
	thread_data* td = (thread_data*)lpvParam;

	BOOL connect;
	connect = ConnectNamedPipe(td->hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
	if (connect == FALSE)
		return -1;

	int numOfRecord = 0;
	char message[MESSAGE_SIZE];

	while(true)
	{
		if (!ReadFile(td->hPipe, message, MESSAGE_SIZE * sizeof(char), NULL, NULL))
		{
			printf("ReadFile failed, %d\n", GetLastError());
			continue;
		}
		sscanf_s(message, "%*s %d", &numOfRecord);
		numOfRecord--;

		if (strncmp(message, "READ ", 5) == 0)
		{
			if (read_option(td, message, numOfRecord) == -1)
				break;
		}
		else if (strncmp(message, "WRITE", 5) == 0) 
		{
			if (write_option(td, numOfRecord, message) == -1)
				break;
		}
		else 
		{
			break;
		}
	}

	FlushFileBuffers(td->hPipe);
	DisconnectNamedPipe(td->hPipe);
	CloseHandle(td->hPipe);
	return 0;
}