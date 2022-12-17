/**========================================================================
 * @author         :  Muhammad Zaid Bari
 * @email          :  zaidbari99@gmail.com
 * @repo           :  https://github.com/zaidbari/cmpe351-project
 * @createdOn      :  December 06, 2022
 * @description    :	CMPE351 Operating systems project for CPU scheduling
 *========================================================================**/

/* -------------------------------- Libraries ------------------------------- */
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cstring>

/* -------------------------- Variable definitaions ------------------------- */
#define DELIMETER ':'
#define INT_MAX 2147483647

/* ---------------------------- Global Variables ---------------------------- */
int TOTAL_PROCESS = 0;
/* ------------------------- Structure definiations ------------------------- */

struct Process
{
	int pid;
	float burst_time;
	float arrival_time;
	int priority;
	float remaining_time;
	float waiting_time;
	Process *next;
	Process *prev;
};

struct filenames
{
	char *output_file_name;
	char *input_file_name;
};

Process *head = NULL; // PROCESSES linked list head

/* ------------------------- function defininations ------------------------- */
filenames getCommandLineArguments(int argc, char *argv[]);
void readInputFile(std::string input_file_name);

int displayMenu(bool premtive, int type, float qt);
int displaySchedulingMenu();

void createProcess(Process **head, int no, float arrival_time, float burst_time, int priority);

void calculateFCFS();
void calculateSJF(bool isPreemptive);

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

	/* ------------------ read input file and create processes ------------------ */
	readInputFile(files.input_file_name);

	int option, type;
	float time_quantum = 0.0f;
	bool isPreemptive = false;

	/* -------------------------- display menu options -------------------------- */
	do
	{
		option = displayMenu(isPreemptive, type, time_quantum);
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
					std::cin >> time_quantum;
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
				calculateFCFS();
			}
			else if (type == 2)
			{
				calculateSJF(isPreemptive);
			}
	} while (option != 4);

	exit(EXIT_SUCCESS);
}

/**
 * @brief get command line arguments
 *
 * @param argc Argument count (contains the number of arguments)
 * @param argv Argument vector (contains argument's values)
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
void readInputFile(std::string input_file_name)
{
	/* ------------------ input/output file stream declarations ----------------- */
	std::ifstream inFile(input_file_name, std::ios::in);
	int number_of_process = 0; // Current Process number while inside the loop | Total number - 1 of processes after reading the file

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
			number_of_process++;

			createProcess(
					&head,							// linked list head
					number_of_process,	// Current process number
					stof(arrival_time), // Arrival time of current process | casting string to float
					stof(burst_time),		// Burst time of current process | casting string to float
					stoi(priority)			// Priority of current process | casting string to integer
			);
		}
		inFile.close();
		TOTAL_PROCESS = number_of_process;
	}
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
	std::cout << " Option> ";
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

	std::cout << " Option> ";
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
void createProcess(Process **head, int process_number, float arrival_time, float burst_time, int priority)
{
	Process *tail = *head, *schedule_node = new Process{process_number, burst_time, arrival_time, priority};

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
 * @return void
 */
void calculateFCFS()
{

	float first_response = 0.0f, total_waiting_time = 0.0f, turnaround_time = 0.0f;
	std::cout << " --------------- Scheduling Method: First Come First Served --------------- " << std::endl;
	std::cout << std::endl;

	std::cout << " Process Waitng times: " << std::endl;
	Process *firstNode = head; // Saving the first node reference

	while (head != NULL)
	{
		if (head->pid == 1)
			first_response = head->arrival_time;

		turnaround_time = (first_response + head->burst_time) - head->arrival_time;
		head->waiting_time = turnaround_time - head->burst_time;
		std::cout << " > P" << head->pid << ": " << head->waiting_time << "ms" << std::endl;

		total_waiting_time += head->waiting_time;
		first_response = first_response + head->burst_time;
		head = head->next;
	}

	head = firstNode; // updating head to point to the first node
	std::cout << std::endl;

	std::cout << " -------------------------------------------------------------------------- " << std::endl;
	std::cout << " > Average waiting time: " << total_waiting_time / TOTAL_PROCESS << "ms" << std::endl;
	std::cout << " -------------------------------------------------------------------------- " << std::endl;
}

/**
 * @brief display results for first come first serve algorithm
 *
 * @param isPreemptive Check for premtive mode
 *
 * @return void
 */
void calculateSJF(bool isPreemptive)
{
	Process *firstNode = head, *shortest = NULL;

	float first_response = 0.0f, total_waiting_time = 0.0f, turnaround_time = 0.0f, waiting_time_of_current_process = 0.0f, total = 0.0f;

	int clock = 0, counter = 0, complete = 0, time_taken, index = 1;

	bool serve = false;
	int MIN_TIME = INT_MAX;

	std::cout << " --------------- Scheduling Method: Shortest Job First ( " << (isPreemptive ? "Preemptive" : "Non-Preemptive") << " ) --------------- " << std::endl;
	std::cout << std::endl;

	std::cout << " Process Waitng times: " << std::endl;
	std::cout << std::endl;

	if (isPreemptive)
	{
		while (complete != TOTAL_PROCESS)
		{
			while (head != NULL)
			{
				if (head->arrival_time <= clock && head->remaining_time < MIN_TIME && head->remaining_time > 0)
				{
					MIN_TIME = head->remaining_time;
					shortest = head;
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

	head = firstNode;

	std::cout << " -------------------------------------------------------------------------- " << std::endl;
	std::cout << " > Average waiting time: " << total_waiting_time / TOTAL_PROCESS << "ms" << std::endl;
	std::cout << " -------------------------------------------------------------------------- " << std::endl;
}