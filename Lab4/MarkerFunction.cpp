#include <Windows.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <vector>

const int sleep_time = 5;

void MarkerFunc(int id, HANDLE start_event, HANDLE continue_event, 
	            HANDLE* stop_event_array, bool* terminate_event_array, 
				CRITICAL_SECTION critical_section, int array_size, std::vector<int> array)
{
	int marked_items = 0;
	std::vector<bool> positions(array_size, false);
	std::cout <<  "Thread " << id + 1 << " created...\n";
	WaitForSingleObject(start_event, INFINITE);
	srand(id + 1);

	while (true)
	{
		int position = rand() % array_size;
		EnterCriticalSection(&critical_section);
		if (array[position])
		{
			Sleep(sleep_time);
			array[position] = id + 1;
			LeaveCriticalSection(&critical_section);
			Sleep(sleep_time);
			positions[position] = true;
			marked_items++;
		}
		else
		{
			LeaveCriticalSection(&critical_section);

			std::cout << std::endl << "Thread num " << id + 1 
				      << "\nElements marked " << marked_items
				      << "\nPosition of unmarkable element " << position 
				      << std::endl;

			SignalObjectAndWait(stop_event_array[id], continue_event, INFINITE, FALSE);
			if (terminate_event_array[id])
			{
				EnterCriticalSection(&critical_section);
				int counter = 0;
				for (int i = 0; i < array_size; i++)
				{
					if (positions[i])
					{
						array[i] = 0;
						counter++;
					}
					if (counter == marked_items)
					{
						break;
					}
				}
				LeaveCriticalSection(&critical_section);
				std::cout << "\nThread " << id + 1 << " is dead...\n";
				SetEvent(stop_event_array[id]);
				break;
			}
		}
	}
}