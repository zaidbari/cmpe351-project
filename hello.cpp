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

filenames getCommandLineArguments(int argc, char *argv[]);
std::vector<schedule_table> readInputFile(std::string input_file_name);

void displayInputFile(std::vector<schedule_table> input_values);
int displayMenu(bool premtive, int type, int qt);
int displaySchedulingMenu();

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

	int option, type, quantum_time = 0;
	bool premtive = false;

	do
	{
		system("clear");
		option = displayMenu(premtive, type, quantum_time);
		if (option == 1)
		{
			do
			{
				type = displaySchedulingMenu();
				if (type == 4)
				{
					std::cout << "Input quantum time value: ";
					std::cin >> quantum_time;
				}
			} while (type != 1 && type != 2 && type != 3 && type != 4);
		}
		if (option == 2)
		{
			premtive = !premtive;
		}
	} while (option != 4 && option != 3);

	exit(EXIT_SUCCESS);
}

/**
 * @brief get command line arguments
 * @param argc Argument count (contains the number of arguments)
 * @param argv  Argument vector (contains argument's values)
 *
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
*	@brief read input file and return a structure
*	@param string input_file_name

*	@return vector<schedule_table>
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
*	@brief display contents of input file
*	@param vector<schedule_table> input_values

*	@return void
*/
void displayInputFile(std::vector<schedule_table> input_values)
{
	for (int i = 0; i < input_values.size(); i++)
	{
		std::cout
				<< " " << input_values[i].burst_time
				<< " " << input_values[i].arrival_time
				<< " " << input_values[i].priority
				<< std::endl;
	}
}

/**
 *	@brief display menu on cli for user input selection
 *	@param bool premtive
 *	@param int type
 *	@param int qt
 *
 *	@return void
 */
int displayMenu(bool premtive = false, int type = 0, int qt = 0)
{

	int option = 0;
	std::string scheduling_method = "None";
	switch (type)
	{
	case 0:
		scheduling_method = "None";
		break;
	case 1:
		scheduling_method = "FCFS";
		break;
	case 2:
		scheduling_method = "SJFS";
		break;
	case 3:
		scheduling_method = "PRIORITY";
		break;
	case 4:
		scheduling_method = "RR";
		break;
	default:
		scheduling_method = "None";
		break;
	}

	std::cout << "\n"
						<< std::endl;
	std::cout << " |------------------------------------ ^ -----------------------------------| " << std::endl;
	std::cout << " |--------------------------------- WELCOME --------------------------------| " << std::endl;
	std::cout << " |------------------------------------ v -----------------------------------| " << std::endl;
	std::cout << "\n"
						<< std::endl;

	std::cout << " [1]: Scheduling Method (" << scheduling_method << ") " << (qt > 0 ? "| QT = " + std::to_string(qt) : " ") << std::endl;
	std::cout << " [2]: Preemptive Mode (" << (premtive ? "YES" : "NO") << ") " << std::endl;
	std::cout << " [3]: Show results" << std::endl;
	std::cout << " [4]: End Program" << std::endl;

	std::cout << "-----------------------------------------------------------------------" << std::endl;
	std::cout << " Option: ";
	std::cin >> option;

	return option;
}

/**
 *	@brief display scheduling type menu
 *
 *	@return int
 */
int displaySchedulingMenu()
{
	int type = 0;
	std::cout << "\n"
						<< std::endl;

	std::cout << " ------------------------ Select a scheduling method ----------------------- " << std::endl;
	std::cout << " [1]: First Come, First Serve " << std::endl;
	std::cout << " [2]: Shortest Job First " << std::endl;
	std::cout << " [3]: Priority " << std::endl;
	std::cout << " [4]: Round-Ribbon " << std::endl;
	std::cout << "-----------------------------------------------------------------------" << std::endl;

	std::cout << " Type: ";
	std::cin >> type;

	std::cout << "\n"
						<< std::endl;

	return type;
}