#pragma once
#include "employee.h"
#include <windows.h>
#include <vector>

const int FILENAME_LEN = 256;
const int EMPLOYEE_NAME_LEN = 10;
const char* PIPENAME = "\\\\.\\pipe\\employee_pipename%d";
const int MESSAGE_SIZE = 16;

typedef struct block_info_struct
{
	std::vector<int> block_counts;
	std::vector<CRITICAL_SECTION> cs_access_to_file;
	std::vector<CRITICAL_SECTION> cs_access_to_write;
	std::vector<HANDLE> mutex_access_to_file;
} block_info;

typedef struct thread_data_struct
{
	HANDLE hPipe;
	employee* file_data;
	block_info* bl_info;
} thread_data;