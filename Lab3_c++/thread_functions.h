#pragma once

using namespace std;

const void min_max_thread_function(int* array, int size, int* min, int* max, chrono::milliseconds timespan) {
	*min = INT32_MAX;
	*max = INT32_MIN;
	for (int i = 0; i < size; i++) {
		if (array[i] < *min)
			*min = array[i];
		std::this_thread::sleep_for(timespan);
		if (array[i] > *max)
			*max = array[i];
	}
	cout << "Array min: " << *min << endl;
	cout << "Array max: " << *max << endl;
}

const void average_thread_function(int* array, int size, double* average, chrono::milliseconds timespan) {
	*average = 0;
	for (int i = 0; i < size; i++)
	{
		*average += array[i];
		std::this_thread::sleep_for(timespan);
	}
	*average /= size;
	cout << "Array average: " << *average << endl;
}