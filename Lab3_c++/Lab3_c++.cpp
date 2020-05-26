#include "pch.h"
#include <iostream>
#include <thread>
#include "thread_functions.h"

using namespace std;

const chrono::milliseconds average_timespan(12);
const chrono::milliseconds min_max_timespan(7);

int main()
{
	int array_size;
	int* array;
	int min, min_index;
	int max, max_index;
	double array_average;

	cout << "Enter array size\n";
	cin >> array_size;
	array = new int[array_size];
	cout << "Enter array elements\n";
	for (int i = 0; i < array_size; i++)
			cin >> array[i];

	thread min_max_thread(min_max_thread_function, array, array_size, &min,&min_index, &max,&max_index, min_max_timespan);
	thread average_thread(average_thread_function, array, array_size, &array_average, average_timespan);

	if (min_max_thread.joinable())
		min_max_thread.join();
	min_max_thread.~thread();

	if (average_thread.joinable())
		average_thread.join();
	average_thread.~thread();

	array[max_index] = array_average;
	array[min_index] = array_average;

	cout << "Min and max were replaced by average: \nNow min is : " << array[min_index] << " Max is: " << array[max_index];

	return 0;
}