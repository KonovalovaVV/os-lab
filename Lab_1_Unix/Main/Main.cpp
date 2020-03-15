#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <fstream>
#include <string>

using namespace std;
const int tiny_array = 4, med_array = 20, big_array = 100;

void print_file(char* file_name, bool binary = false)
{
	cout << "\n\n" << file_name << ":\n";
	ifstream fin;
	if (binary)
		fin.open(file_name, ios::binary);
	else
		fin.open(file_name);
	while (!fin.eof())
	{
		string tmp;
		getline(fin, tmp);
		cout << tmp << endl;
	}
	fin.close();

	cout << endl << endl;
}

int start_process_wait(char* const params[], char* process_name)
{
	pid_t pid = vfork();

	if (pid == -1) {
		fprintf(stderr, "Unable to fork\n");
	}
	else if (pid > 0)
	{
		int status;
		waitpid(pid, &status, 0);
		int exit_code = WEXITSTATUS(status);
		if (exit_code)
		{
			printf("Exit code = %d\n", exit_code);
			return 1;
		}
	}
	else if (pid == 0) {
		if (execv(process_name, params) == -1)
		{
			fprintf(stderr, "Unable to exec\n");
			exit(pid);
			return 1;
		}
	}
	return 0;
}

int main()
{
	char binary_file_name[med_array];
	cout << "Enter name of bin file\n";
	cin.getline(binary_file_name, med_array);

	char num[tiny_array];
	cout << "Enter num of records\n";
	cin.getline(num, tiny_array);

	char* creator_parameters[2];
	creator_parameters[0] = binary_file_name;
	creator_parameters[1] = num;
	if (start_process_wait(creator_parameters, (char*)"Creator"))
	{
		return 1;
	}
	print_file(binary_file_name, true);

	char report_file_name[med_array];
	printf("\nEnter name of report file\n");
	cin.getline(report_file_name, med_array);

	char salary[tiny_array];
	cout << "Enter salary \n";
	cin.getline(salary, tiny_array);

	char * reporter_parameters[3];
	reporter_parameters[0] = binary_file_name;
	reporter_parameters[1] = report_file_name;
	reporter_parameters[2] = salary;
	if (start_process_wait(reporter_parameters, (char*)"Reporter"))
	{
		return 1;
	}
	print_file(report_file_name, false);
	return 0;
}
