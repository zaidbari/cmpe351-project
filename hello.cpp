/* -------------------------------- Libraries ------------------------------- */
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <fstream>
#include <vector>
#include <sstream>

/* ------------------------------- Namespaces ------------------------------- */
// using namespace std;

/* -------------------------- Variable definitaions ------------------------- */
#define DELIMETER ':'

/* ------------------------- Structure definiations ------------------------- */
struct schedule_table
{
	int burst_time;
	int arrival_time;
	int priority;
};

struct filenames
{
	char *output_file;
	char *input_file;
};

/**
 * @brief get command line arguments
 * @param argc
 * @param argv
 * @return filenames structure
 */
filenames getCommandLineArguments(int argc, char *argv[])
{

	char *input_file_name, *output_file_name;

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
*	@brief
*	@param input_file_name

*	@return std::vector<schedule_table>
*/
std::vector<schedule_table> readInputFile(std::string input_file_name)
{
	/* ------------------ input/output file stream declarations ----------------- */
	std::ifstream inFile(input_file_name);

	/* ---------------------- vector to store input values ---------------------- */
	std::vector<schedule_table> values;

	if (inFile.is_open())
	{
		std::string line;
		while (std::getline(inFile, line))
		{
			schedule_table row;

			std::stringstream ss(line);
			std::string burst_time, arrival_time, priority;

			std::getline(ss, burst_time, DELIMETER);
			std::getline(ss, arrival_time, DELIMETER);
			std::getline(ss, priority, DELIMETER);

			/* ------------------ row of input values from single line ------------------ */
			row.burst_time = stoi(burst_time);
			row.arrival_time = stoi(arrival_time);
			row.priority = stoi(priority);

			values.push_back(row);
		}

		inFile.close();
	}

	return values;
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
	std::vector<schedule_table> input_values = readInputFile(input_file_name);

	for (int i = 0; i < input_values.size(); i++)
	{
		std::cout
				<< " " << input_values[i].burst_time
				<< " " << input_values[i].arrival_time
				<< " " << input_values[i].priority
				<< std::endl;
	}

	exit(EXIT_SUCCESS);
}
