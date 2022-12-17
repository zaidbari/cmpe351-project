/**========================================================================
 * @author         :  Muhammad Zaid Bari
 * @email          :  zaidbari99@gmail.com
 * @repo           :  https://github.com/zaidbari/cmpe351-project
 * @createdOn      :  December 06, 2022
 * @description    :	CMPE351 Operating systems project for CPU scheduling
 *========================================================================**/

/* -------------------------------- Libraries ------------------------------- */
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <fstream>
#include <vector>
#include <sstream>
// #include <cstdint>
#include <filesystem>

/* -------------------------- Variable definitaions ------------------------- */
#define DELIMETER ':'
#define INT_MAX 2147483647
/* ------------------------- Structure definiations ------------------------- */

typedef struct process
{
	float burst_time, arrival_time, remaining_time, waiting_time;
	int number, priority;
	process *next;

} PROCESS_LIST;

struct read_output
{
	process header;
	int number_of_process;
};

struct filenames
{
	char *output_file_name;
	char *input_file_name;
};

PROCESS_LIST *header = NULL; // PROCESSES linked list head

/* ------------------------- function defininations ------------------------- */
filenames getCommandLineArguments(int argc, char *argv[]);
read_output readInputFile(std::string input_file_name);

int displayMenu(bool premtive, int type, float qt);
int displaySchedulingMenu();

void createSchedule(PROCESS_LIST **head, int no, float arrival_time, float burst_time, int priority);

void calculateFCFS(int process);
void calculateSJF(int total_number_of_processes, bool isPreemptive);

/**
 * @brief Main entry point
 *
 * @param argc Argument count (contains the number of arguments)
 * @param argv  Argument vector (contains argument's values)
 *
 * @return int EXIT_SUCCESS = 1
 */
int main(int argc, char *argv[])
{

	/* -------------- get command line arguments and store results -------------- */
	filenames files = getCommandLineArguments(argc, argv);

	/* ----------------- check if input file exists in directory ---------------- */
	if (!std::filesystem::exists(files.input_file_name))
	{
		system("clear");

		std::cout << " -------------------------------------------------------------------------- " << std::endl;
		std::cerr << " ERROR: "
							<< "\"" << files.input_file_name << "\""
							<< " doesn't exists or is un-reachable" << std::endl;
		std::cout << " -------------------------------------------------------------------------- " << std::endl;

		exit(EXIT_FAILURE);
	}

	read_output output = readInputFile(files.input_file_name);

	int option, type;
	float quantum_time = 0;
	bool isPreemptive = false;

	/* -------------------------- display menu options -------------------------- */
	do
	{
		option = displayMenu(isPreemptive, type, quantum_time);
		if (option == 1)
		{
			/* ------------------------- display scheduling menu ------------------------ */
			system("clear");
			do
			{
				type = displaySchedulingMenu();
				/* -------------------- if round-robin, get quantam time -------------------- */
				if (type == 4)
				{
					std::cout << " Input quantum time value: ";
					std::cin >> quantum_time;
				}
				system("clear");
			} while (type != 1 && type != 2 && type != 3 && type != 4);
		}

		if (option == 2)
		{
			isPreemptive = !isPreemptive;
			system("clear");
		}
		if (option == 3)
			if (type == 1)
			{
				system("clear");
				calculateFCFS(output.number_of_process);
			}
			else if (type == 2)
			{
				calculateSJF(output.number_of_process, isPreemptive);
			}
	} while (option != 4);

	exit(EXIT_SUCCESS);
}

/**
 * @brief get command line arguments
 *
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

	return filenames{output_file_name, input_file_name};
}

/**
*	@brief read input file and return a structure
*
*	@param input_file_name

*	@return void
*/
read_output readInputFile(std::string input_file_name)
{

	/* ------------------ input/output file stream declarations ----------------- */
	std::ifstream inFile(input_file_name, std::ios::in);
	int number_of_process = 1; // Current Process number while inside the loop | Total number - 1 of processes after reading the file

	if (inFile.is_open())
	{
		std::string line;

		while (std::getline(inFile, line))
		{
			std::stringstream ss(line);
			std::string burst_time, arrival_time, priority;
			std::getline(ss, burst_time, DELIMETER);
			std::getline(ss, arrival_time, DELIMETER);
			std::getline(ss, priority, DELIMETER);

			createSchedule(
					&header,						// linked list head
					number_of_process,	// Current process number
					stof(arrival_time), // Arrival time of current process | casting string to float
					stof(burst_time),		// Burst time of current process | casting string to float
					stoi(priority)			// Priority of current process | casting string to integer
			);

			number_of_process++;
		}
		inFile.close();
	}

	return read_output{*header, (number_of_process - 1)};
}

/**
 *	@brief display menu on cli for user input selection
 *
 *	@param premtive boolean to check if preemptive type is selected
 *	@param type type of scheduling method selected enum[1,2,3,4]
 *	@param qt quantam time value if type = 4 | Round-Robin Algorithm
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
		scheduling_method = "SJF";
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

	std::cout << " --------------------------------- WELCOME -------------------------------- " << std::endl;
	std::cout << std::endl;

	std::cout << " [1]: Scheduling Method (" << scheduling_method << ") " << (qt > 0 ? "| QT = " + std::to_string(qt) : " ") << std::endl;
	std::cout << " [2]: Preemptive Mode (" << (premtive ? "YES" : "NO") << ") " << std::endl;
	std::cout << " [3]: Show results" << std::endl;
	std::cout << " [4]: End Program" << std::endl;

	std::cout << std::endl;
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
	std::cout << " ------------------------ Select a scheduling method ----------------------- " << std::endl;
	std::cout << std::endl;

	std::cout << " [1]: First Come, First Serve " << std::endl;
	std::cout << " [2]: Shortest Job First " << std::endl;
	std::cout << " [3]: Priority " << std::endl;
	std::cout << " [4]: Round-Ribbon " << std::endl;
	std::cout << std::endl;

	std::cout << "-----------------------------------------------------------------------" << std::endl;

	std::cout << " Type: ";
	std::cin >> type;

	std::cout << "\n"
						<< std::endl;

	return type;
}

/**
 * @brief create a process linked list with dynamic memory allocation
 *
 * @param head <schedule> struct | head of the linked list
 * @param process_number	Process number
 * @param arrival_time Arrival time in miliseconds
 * @param burst_time Burst time in miliseconds
 * @param priority Priority of the process
 *
 * @return void
 */
void createSchedule(PROCESS_LIST **head, int process_number, float arrival_time, float burst_time, int priority)
{

	PROCESS_LIST *schedule_node, *tail = *head;
	schedule_node = (PROCESS_LIST *)malloc(sizeof(PROCESS_LIST));

	schedule_node->number = process_number;
	schedule_node->arrival_time = arrival_time;
	schedule_node->burst_time = burst_time;
	schedule_node->priority = priority;
	schedule_node->remaining_time = schedule_node->burst_time;
	schedule_node->next = NULL;

	if (*head == NULL)
		*head = schedule_node;
	else
	{
		while (tail->next != NULL)
			tail = tail->next;
		tail->next = schedule_node;
	}
}

/**
 * @brief display results for first come first serve algorithm
 *
 * @param head head of linked list pointer
 * @param total_number_of_processes Total number of processes from input file
 *
 * @return void
 */
void calculateFCFS(int total_number_of_processes)
{

	float first_response = 0.0f, total_waiting_time = 0.0f, turnaround_time = 0.0f;
	std::cout << " --------------- Scheduling Method: First Come First Served --------------- " << std::endl;
	std::cout << std::endl;

	std::cout << " Process Waitng times: " << std::endl;
	PROCESS_LIST *firstNode = header;

	while (header != NULL)
	{
		if (header->number == 1)
			first_response = header->arrival_time;

		turnaround_time = (first_response + header->burst_time) - header->arrival_time;
		header->waiting_time = turnaround_time - header->burst_time;
		std::cout << " > P" << header->number << ": " << header->waiting_time << "ms" << std::endl;

		total_waiting_time += header->waiting_time;
		first_response = first_response + header->burst_time;

		header = header->next;
	}

	header = firstNode;
	std::cout << std::endl;

	std::cout << " -------------------------------------------------------------------------- " << std::endl;
	std::cout << " > Average waiting time: " << total_waiting_time / total_number_of_processes << "ms" << std::endl;
	std::cout << " -------------------------------------------------------------------------- " << std::endl;
}

/**
 * @brief display results for first come first serve algorithm
 *
 * @param head head of linked list pointer
 * @param total_number_of_processes Total number of processes from input file
 *
 * @return void
 */
void calculateSJF(int total_number_of_processes, bool isPreemptive)
{
	float first_response = 0.0f, total_waiting_time = 0.0f, turnaround_time = 0.0f, waiting_time_of_current_process = 0.0f, total = 0.0f;

	int clock = 0, counter = 0, complete = 0, time_taken, index = 1;

	bool serve = false;
	int MIN_TIME = INT_MAX;

	std::cout << " --------------- Scheduling Method: Shortest Job First ( " << (isPreemptive ? "Preemptive" : "Non-Preemptive") << " ) --------------- " << std::endl;
	std::cout << std::endl;

	std::cout << " Process Waitng times: " << std::endl;
	std::cout << std::endl;
	PROCESS_LIST *firstNode = header, *shortest = NULL;

	if (isPreemptive)
	{
		while (complete != total_number_of_processes)
		{
			while (header != NULL)
			{
				if (header->arrival_time <= clock && header->remaining_time < MIN_TIME && header->remaining_time > 0)
				{
					MIN_TIME = header->remaining_time;
					shortest = header;
					serve = true;
				}

				index++;
			}

			if (serve == false)
			{
				clock++;
				continue;
			}

			shortest->remaining_time--;
			MIN_TIME = shortest->remaining_time;
			if (MIN_TIME == 0)
				MIN_TIME = INT_MAX;

			if (shortest->remaining_time == 0)
			{
				complete++;
				serve = false;
				turnaround_time = clock + 1;
				shortest->waiting_time = turnaround_time - shortest->burst_time - shortest->arrival_time;
				if (shortest->waiting_time < 0)
				{
					shortest->waiting_time = 0;
				}

				total_waiting_time = total_waiting_time + shortest->waiting_time;
			}
			clock++;
		}
	}

	header = firstNode;

	std::cout << " -------------------------------------------------------------------------- " << std::endl;
	std::cout << " > Average waiting time: " << total_waiting_time / total_number_of_processes << "ms" << std::endl;
	std::cout << " -------------------------------------------------------------------------- " << std::endl;
}