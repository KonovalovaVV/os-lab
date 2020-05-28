#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <thread>
#include <vector>

const int wait_time = 150;

void MarkerFunc(int number, HANDLE start_event, HANDLE continue_event,
				HANDLE* stop_event_array, bool* terminate_event_array,
				CRITICAL_SECTION critical_section, int array_size, std::vector<int> array);

int main()
{
	int array_size = 0;
	int threads_amount = 0;
	HANDLE start_event;
	HANDLE continue_event;
	HANDLE* stop_event_array;
	bool* terminate_event_array;
	CRITICAL_SECTION critical_section;

	std::cout << "Enter array size ";
	std::cin >> array_size;
	std::cout << "Enter number of marker threads ";
	std::cin >> threads_amount;

	std::vector<int> array(array_size, 0);
	std::thread* thread_array = new std::thread[threads_amount];
	stop_event_array = new HANDLE[threads_amount];
	terminate_event_array = new bool[threads_amount];

	for (int i = 0; i < threads_amount; i++)
	{
		terminate_event_array[i] = false;
	}

	start_event = CreateEvent(NULL, TRUE, FALSE, NULL);
	continue_event = CreateEvent(NULL, TRUE, FALSE, NULL);


	InitializeCriticalSection(&critical_section);

	for (int i = 0; i < threads_amount; i++)
	{
		stop_event_array[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		thread_array[i] = std::thread(MarkerFunc, i, start_event, continue_event, 
						  stop_event_array, terminate_event_array, critical_section, array_size, array);
	}

	SetEvent(start_event);

	int alive_thread_amount = threads_amount;
	while (true)
	{
		WaitForMultipleObjects(threads_amount, stop_event_array, TRUE, wait_time);
		std::cout << "\nThreads stopped...\n";

		for (int i = 0; i < array_size; i++)
		{
			std::cout << array[i] << " ";
		}

		if (alive_thread_amount == 0)
		{
			std::cout << "\nNo threads are available....\n";
			break;
		}

		std::cout << "\nPlease, enter thread number to terminate : \n";
		int number;
		std::cin >> number;
		terminate_event_array[number - 1] = TRUE;
		ResetEvent(continue_event);

		alive_thread_amount--;
	}

	DeleteCriticalSection(&critical_section);
	for (int i = 0; i < threads_amount; i++)
	{
		CloseHandle(stop_event_array[i]);
	}
	
	CloseHandle(start_event);
	CloseHandle(continue_event);
	return 0;
}