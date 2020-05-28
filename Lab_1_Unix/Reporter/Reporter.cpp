#include <iostream>
#include <string.h>

struct employee
{
	int num;
	char name[10];
	double hours;
};

const int big_string = 300;

int main(int argc, char** argv)
{
	if (argc != 4)
	{
		std::cout << "ERROR: wrong number of	parameters\n";
		return 1;
	}

	double salary = atof(argv[2]);

	FILE* bin_file = NULL;
	FILE* report_file = NULL;

	bin_file = fopen(argv[0], "r");
	report_file = fopen(argv[1], "w");

	if (bin_file == NULL)
	{
		std::cout << "Bin file is not found\n";
		fclose(report_file);
		return 2;
	}

	char out_string[big_string];
	employee tmp;

	sprintf(out_string, "\t\tReport %s\n№ Name Hours Salary\n", argv[1]);
	fwrite(out_string, strlen(out_string), 1, report_file);

	while (fread(&tmp, sizeof(employee), 1, bin_file))
	{
		sprintf(out_string, "%d %s %0.2f %0.2f\n",
			tmp.num, tmp.name, tmp.hours, tmp.hours*salary);
		fwrite(out_string, strlen(out_string), 1, report_file);
	}

	fclose(report_file);
	fclose(bin_file);
	return 0;
}

