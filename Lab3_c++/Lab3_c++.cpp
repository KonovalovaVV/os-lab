#include "pch.h"
#include <iostream>
#include <thread>
#include "ThreadFunctions.h"

using namespace std;

int main()
{
	int ArraySize;
	int* Array;
	int Min, Max;
	double ArrayAverage;

	cout << "Enter array size\n";
	cin >> ArraySize;
	Array = new int[ArraySize];
	cout << "Enter array elements\n";
	for (int i = 0; i < ArraySize; i++)
			cin >> Array[i];

	thread MinMax(MinMaxThreadFunction, Array, ArraySize, &Min, &Max, MinMaxTimespan);
	thread Average(AverageThreadFunction, Array, ArraySize, &ArrayAverage, AverageTimespan);

	if (MinMax.joinable())
	MinMax.join();
	MinMax.~thread();		

	if (Average.joinable())
	Average.join();
	Average.~thread();

	Min = ArrayAverage;
	Max = ArrayAverage;

	cout << "Min and Max were by average: \nMin :" << Min << " Max: " << Max;

	return 0;
}

void MinMaxThreadFunction(int* array, int size, int* min, int* max, chrono::milliseconds timespan) {
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

void AverageThreadFunction(int* array, int size, double* average, chrono::milliseconds timespan) {
	*average = 0;
	for (int i = 0; i < size; i++)
	{
		*average += array[i];
		std::this_thread::sleep_for(timespan);
	}
	*average /= size;
	cout << "Array average: " << *average << endl;
}