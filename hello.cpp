/* -------------------------------- Libraries ------------------------------- */
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <cstdint>

/* ------------------------------- Namespaces ------------------------------- */
// using namespace std;

/* -------------------------- Variable definitaions ------------------------- */
#define DELIMETER ':'

/* ------------------------- Structure definiations ------------------------- */

typedef struct schedule
{
	float burst_time, arrival_time, turnaround_time, relative_delay, waiting_time, response_time, process_completed;
	int number, priority;
	struct schedule *next;
} SCHEDULE_NODE;

struct read_output
{
	schedule header;
	int number_of_process;
};

struct filenames
{
	char *output_file;
	char *input_file;
};

/* -------------------------------------------------------------------------- */
/*                           function defininations                           */
/* -------------------------------------------------------------------------- */
filenames getCommandLineArguments(int argc, char *argv[]);
read_output readInputFile(std::string input_file_name);

int displayMenu(bool premtive, int type, float qt);
int displaySchedulingMenu();

void displayFCFS(SCHEDULE_NODE *p, int process);
void create_schedule(SCHEDULE_NODE **sc, int no, float arrival_time, float burst_time, int priority, float *first_response);

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
	auto [header, number_of_process] = readInputFile(input_file_name);

	int option, type;
	float quantum_time = 0;
	bool premtive = false;

	do
	{
		system("clear");

		option = displayMenu(premtive, type, quantum_time);
		if (option == 1)
		{
			system("clear");

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
			premtive = !premtive;

		if (option == 3)
		{
			if (type == 1)
			{
				system("clear");
				displayFCFS(&header, number_of_process);
			}
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

*	@return void
*/
read_output readInputFile(std::string input_file_name)
{
	SCHEDULE_NODE *header = NULL;

	/* ------------------ input/output file stream declarations ----------------- */
	std::ifstream inFile(input_file_name, std::ios::in);
	int number_of_process = 0;

	if (inFile.is_open())
	{
		std::string line;

		while (std::getline(inFile, line))
		{
			std::stringstream ss(line);
			std::string burst_time, arrival_time, priority;
			float first_response;

			std::getline(ss, burst_time, DELIMETER);
			std::getline(ss, arrival_time, DELIMETER);
			std::getline(ss, priority, DELIMETER);

			if (number_of_process == 0)
				first_response = stof(arrival_time);

			create_schedule(&header, number_of_process, stof(arrival_time), stof(burst_time), stoi(priority), &first_response);

			number_of_process++;
		}
		inFile.close();
	}

	return read_output{*header, number_of_process};
}

/**
 *	@brief display menu on cli for user input selection
 *	@param bool premtive
 *	@param int type
 *	@param int qt
 *
 *	@return void
 */
int displayMenu(bool premtive = false, int type = 0, float qt = 0)
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

/**
 * @brief
 * @param sc <schedule> struct
 * @param number	Process number
 * @param arrival_time
 * @param burst_time
 * @param priority
 * @param first_response Number == 1 ? *first_response = arrival_time : *first_response + burst_time
 */
void create_schedule(SCHEDULE_NODE **sc, int number, float arrival_time, float burst_time, int priority, float *first_response)
{

	SCHEDULE_NODE *schedule_node, *r = *sc;
	schedule_node = (SCHEDULE_NODE *)malloc(sizeof(SCHEDULE_NODE));

	schedule_node->number = number;
	schedule_node->arrival_time = arrival_time;
	schedule_node->burst_time = burst_time;
	schedule_node->priority = priority;
	schedule_node->response_time = *first_response - arrival_time;
	schedule_node->process_completed = *first_response + burst_time;
	schedule_node->turnaround_time = schedule_node->process_completed - arrival_time;
	schedule_node->waiting_time = schedule_node->turnaround_time - burst_time;
	schedule_node->relative_delay = schedule_node->turnaround_time / burst_time;

	*first_response = *first_response + burst_time;

	schedule_node->next = NULL;
	if (*sc == NULL)
	{
		*sc = schedule_node;
	}
	else
	{
		while (r->next != NULL)
		{
			r = r->next;
		}
		r->next = schedule_node;
	}
}

void displayFCFS(SCHEDULE_NODE *processes, int number_of_processes)
{
	float total_waiting_time = 0;
	std::cout << " --------------- Scheduling Method: First Come First Served --------------- " << std::endl;
	std::cout << " Process Waitng times: " << std::endl;

	while (processes != NULL)
	{
		std::cout << " P[" << processes->number << "]: " << processes->waiting_time << "ms" << std::endl;
		total_waiting_time += processes->waiting_time;
		processes = processes->next;
	}
	std::cout << " -------------------------------------------------------------------------- " << std::endl;
	std::cout << " Average waiting time: " << total_waiting_time / number_of_processes << "ms" << std::endl;
	std::cout << " -------------------------------------------------------------------------- " << std::endl;
}
