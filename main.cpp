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
#include <vector>		 //Required for using vectors
#include <algorithm> //Required for sort() library function

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
	float turnaround_time;
	float completion_time;
	Process *next;
	Process *prev;
};

struct filenames
{
	char *output_file_name;
	char *input_file_name;
};

Process *head = nullptr; // PROCESSES linked list head

/* ---------------------------- sorting function ---------------------------- */
Process *sortLinkedList(Process *head, std::string method);

/* ------------------------- function defininations ------------------------- */
filenames getCommandLineArguments(int argc, char *argv[]);
void readInputFile(std::string input_file_name);

int displayMenu(bool premtive, int type, float qt);
int displaySchedulingMenu();

void createProcess(Process **head, int pid, float arrival_time, float burst_time, int priority);

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
				system("clear");
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
 * @param head <Process> struct | head of the linked list
 * @param pid	Process number
 * @param arrival_time Arrival time in miliseconds
 * @param burst_time Burst time in miliseconds
 * @param priority Priority of the process
 *
 * @return void
 */
void createProcess(Process **head, int pid, float arrival_time, float burst_time, int priority)
{
	Process *tail = *head, *current_node = new Process{pid, burst_time, arrival_time, priority};

	if (*head == NULL)
		*head = current_node;
	else
	{
		while (tail->next != NULL)
			tail = tail->next;
		tail->next = current_node;
	}
}

/**
 * @brief display results for first come first serve algorithm
 *
 * @return void
 */
void calculateFCFS()
{
	Process *current_node = sortLinkedList(head, "fcfs");

	float first_response = 0.0f, total_waiting_time = 0.0f, turnaround_time = 0.0f;
	std::cout << " --------------- Scheduling Method: First Come First Served --------------- " << std::endl;
	std::cout << std::endl;

	std::cout << " Process Waitng times [ms]: " << std::endl;
	while (current_node != nullptr)
	{
		if (current_node->pid == 1)
			first_response = current_node->arrival_time;

		turnaround_time = (first_response + current_node->burst_time) - current_node->arrival_time;
		current_node->waiting_time = turnaround_time - current_node->burst_time;
		std::cout << " > P" << current_node->pid << ": " << current_node->waiting_time << "ms" << std::endl;

		total_waiting_time += current_node->waiting_time;
		first_response = first_response + current_node->burst_time;
		current_node = current_node->next;
	}

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
	Process *current_node = sortLinkedList(head, "sjf");
	float total_waiting_time = 0.0f, waiting_time_of_current_process = 0.0f, elapsed_time = 0.0f;

	std::cout << " --------------- Scheduling Method: Shortest Job First ( " << (isPreemptive ? "Preemptive" : "Non-Preemptive") << " ) --------------- " << std::endl;
	std::cout << std::endl;

	std::cout << " Process Waitng times [ms]: " << std::endl;
	while (current_node != nullptr)
	{
		current_node->waiting_time = elapsed_time - current_node->arrival_time;
		std::cout << " > P" << current_node->pid << ": " << current_node->waiting_time << std::endl;

		total_waiting_time += current_node->waiting_time;
		elapsed_time += current_node->burst_time;
		current_node = current_node->next;
	}

	std::cout << " -------------------------------------------------------------------------- " << std::endl;
	std::cout << " > Average waiting time: " << total_waiting_time / TOTAL_PROCESS << "ms" << std::endl;
	std::cout << " -------------------------------------------------------------------------- " << std::endl;
}

void swapNodes(Process *current_node, Process *compare_node)
{
	std::swap(current_node->arrival_time, compare_node->arrival_time);
	std::swap(current_node->burst_time, compare_node->burst_time);
	std::swap(current_node->pid, compare_node->pid);
	std::swap(current_node->priority, compare_node->priority);
	std::swap(current_node->remaining_time, compare_node->remaining_time);
	std::swap(current_node->waiting_time, compare_node->waiting_time);
	std::swap(current_node->turnaround_time, compare_node->turnaround_time);
	std::swap(current_node->completion_time, compare_node->completion_time);
}

// Function to sort the linked list according to arrival time and burst time and return a pointer to the head of the sorted copy
Process *sortLinkedList(Process *list, std::string method)
{
	// Check if the linked list is empty
	if (list == nullptr)
	{
		return nullptr;
	}

	// Create a copy of the linked list
	Process *copy_head = nullptr;
	Process *copy_tail = nullptr;
	Process *current_node = list;

	while (current_node != nullptr)
	{
		if (copy_head == nullptr)
		{
			copy_head = new Process{current_node->pid, current_node->burst_time, current_node->arrival_time, current_node->priority};
			copy_tail = copy_head;
		}
		else
		{
			copy_tail->next = new Process{current_node->pid, current_node->burst_time, current_node->arrival_time, current_node->priority};
			copy_tail->next->prev = copy_tail;
			copy_tail = copy_tail->next;
		}
		current_node = current_node->next;
	}

	// Initialize variables to track the nodes being compared
	current_node = copy_head;
	Process *compare_node = current_node->next;

	// Iterate through the linked list
	while (current_node != nullptr)
	{
		compare_node = current_node->next;
		while (compare_node != nullptr)
		{
			if (method.compare("sjf") == 0)
			{
				// Compare the arrival time of the two nodes
				if (current_node->arrival_time > compare_node->arrival_time)
				{
					swapNodes(current_node, compare_node);
				}
				// If the arrival times are the same, compare the burst times
				else if (current_node->arrival_time && current_node->burst_time > compare_node->burst_time)
				{
					swapNodes(current_node, compare_node);
				}
			}

			if (method.compare("pid") == 0)
			{
				if (current_node->pid > compare_node->pid)
				{
					// Swap the nodes if the arrival time of the compare node is lower
					swapNodes(current_node, compare_node);
				}
			}

			if (method.compare("fcfs") == 0)
			{
				if (current_node->arrival_time > compare_node->arrival_time)
				{
					// Swap the nodes if the arrival time of the compare node is lower
					swapNodes(current_node, compare_node);
				}
			}

			compare_node = compare_node->next;
		}
		current_node = current_node->next;
	}

	// Return a pointer to the head of the sorted copy
	return copy_head;
}