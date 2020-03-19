#include <Windows.h>
#include <stdio.h>
#include <math.h>
#include <iostream>

const int sleep_time = 5;

void MarkerFunc(int number, HANDLE start_event, HANDLE continue_event, 
	            HANDLE* stop_event_array, bool* terminate_event_array, 
				CRITICAL_SECTION critical_section, int array_size, int* array)
{
	int marked_items = 0;
	int* positions = new int[array_size];
	for (int i = 0; i < array_size; i++)
	{
		positions[i] = -1;
	}
	std::cout <<  "Thread " << number + 1 << " created...\n";
	WaitForSingleObject(start_event, INFINITE);
	srand(number + 1);

	while (true)
	{
		int position = rand() % array_size;
		EnterCriticalSection(&critical_section);
		if (marked_items < array_size && array[position] == 0)
		{
			Sleep(sleep_time);
			array[position] = number + 1;
			LeaveCriticalSection(&critical_section);
			Sleep(sleep_time);
			positions[position] = 0;
			marked_items++;
			
		}
		else
		{
			LeaveCriticalSection(&critical_section);

			std::cout << "\nThread num " << number + 1 << "\n";
			std::cout << "Elements marked " << marked_items << "\n";
			std::cout << "Position of unmarkable element " << position << "\n";

			SignalObjectAndWait(stop_event_array[number], continue_event, INFINITE, FALSE);
			if (terminate_event_array[number])
			{
				EnterCriticalSection(&critical_section);
				for (int i = 0; i < array_size; i++)
				{
					if (positions[i] != -1)
					{
						array[i] = 0;
					}
				}
				LeaveCriticalSection(&critical_section);
				std::cout << "\nThread " << number + 1 << " is dead...\n";
				SetEvent(stop_event_array[number]);
				break;
			}
		}
	}
}