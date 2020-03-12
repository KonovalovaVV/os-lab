#pragma once

const std::chrono::milliseconds AverageTimespan(12);
const std::chrono::milliseconds MinMaxTimespan(7);

void MinMaxThreadFunction(int* array, int size, int* min, int* max, std::chrono::milliseconds timespan);
void AverageThreadFunction(int* array, int size, double* average, std::chrono::milliseconds timespan);
