/* -------------------------------- Libraries ------------------------------- */
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <fstream>
#include <vector>
#include <sstream>

/* ------------------------------- Namespaces ------------------------------- */
// using namespace std;

/* -------------------------- variable definitaions ------------------------- */
#define DELIMETER ':'

auto getCommandLineArguments(int argc, char *argv[])
{

	char *input_file_name, *output_file_name;
	struct filenames
	{
		char *output_file;
		char *input_file;
	};

	for (int i = 1; i < argc; i++)
		if (i + 1 != argc)
		{
			if (strcmp(argv[i], "-f") == 0)
				input_file_name = argv[i + 1];
			if (strcmp(argv[i], "-o") == 0)
				output_file_name = argv[i + 1];
			i++;
		}

	return filenames{input_file_name, output_file_name};
}

/**
 * @brief Main entry point
 * @param argc Argument count (contains the number of arguments)
 * @param argv  Argument vector (contains argument's values)
 *
 * @return int EXIT_SUCCESS = 1
 */
int main(int argc, char *argv[])
{

	/* -------------- get command line arguments and store results -------------- */
	auto [input_file_name, output_file_name] = getCommandLineArguments(argc, argv);

	/* ------------------ input/output file stream declarations ----------------- */
	std::ifstream inFile(input_file_name);

	/* ---------------------- vector to store input values ---------------------- */
	std::vector<std::vector<int>> values;

	if (inFile.is_open())
	{
		std::string line;
		while (std::getline(inFile, line))
		{

			std::vector<int> row;

			std::stringstream ss(line);
			std::string burst_time, arrival_time, priority;

			std::getline(ss, burst_time, DELIMETER);
			std::getline(ss, arrival_time, DELIMETER);
			std::getline(ss, priority, DELIMETER);

			/* ------------------ row of input values from single line ------------------ */
			row.push_back(stoi(burst_time));
			row.push_back(stoi(arrival_time));
			row.push_back(stoi(priority));

			values.push_back(row);
		}

		inFile.close();
	}

	for (int i = 0; i < values.size(); i++)
	{
		for (int j = 0; j < values[i].size(); j++)
			std::cout << values[i][j] << " ";
		std::cout << std::endl;
	}

	exit(EXIT_SUCCESS);
}
