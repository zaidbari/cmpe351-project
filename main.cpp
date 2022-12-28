/**========================================================================
 * @author         :  Muhammad Zaid Bari
 * @email          :  zaidbari99@gmail.com
 * @repo           :  https://github.com/zaidbari/cmpe351-project
 * @createdOn      :  December 06, 2022
 * @description    :	CMPE351 Operating systems project for CPU scheduling
 *========================================================================**/

/* ------------------------------ Header files ------------------------------ */
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cstring>
#include <algorithm>

/* -------------------------- Variable definitaions ------------------------- */
#define DELIMETER ':'
#define INT_MAX 2147483647
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
	float elapsed_time;
	bool completed;
	Process *next;
	Process *prev;
};

struct filenames
{
	char *output_file_name;
	char *input_file_name;
};

Process *head = nullptr; // PROCESSES linked list head

/* ------------------------- function defininations ------------------------- */
filenames getCommandLineArguments(int argc, char *argv[]);
void readInputFile(std::string input_file_name);

int displayMenu(bool premtive, int type, float qt);
int displaySchedulingMenu();

void createProcess(Process **head, int pid, float arrival_time, float burst_time, int priority);

void calculateFCFS();
void calculateSJFNonPremptive();
void calculateSJFPremptive();
void calculatePriority();

/**
 * @brief Main entry point
 *
 * @param argc Argument count (contains the number of arguments)
 * @param argv  Argument vector (contains argument's values)
 *
 * @return int EXIT_SUCCESS = 0
 */
int main(int argc, char *argv[])
{
	system("clear");

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

		switch (option)
		{
		case 1: // Selection of scheduling method
			system("clear");
			do
			{
				type = displaySchedulingMenu();
				if (type == 4) // if round-robin, get quantam time
				{
					std::cout << " Input quantum time value: ";
					std::cin >> time_quantum;
				}
				system("clear");
			} while (type != 1 && type != 2 && type != 3 && type != 4);
			break;

		case 2: // Boolean for setting method to preemptive or non-preemptive
			isPreemptive = !isPreemptive;
			system("clear");
			break;

		case 3: // case for displaying results
			switch (type)
			{
			case 1: // case fo displaying first come first serve algorithm
				system("clear");
				calculateFCFS();
				break;
			case 2: // case for displaying shortest job first algorithm
				system("clear");
				if (!isPreemptive)
					calculateSJFNonPremptive();
				else
					calculateSJFPremptive();
				break;
			case 3: // case for displaying priority scheduling algorithm
				system("clear");
				calculatePriority();
				break;
			}
			break;

		default:
			break;
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
	std::cout << " [4]: End Program (Run all algos, display and write to file)" << std::endl;

	std::cout << std::endl;
	std::cout << " -------------------------------------------------------------------------- " << std::endl;
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

	std::cout << " --------------------------------------------------------------------------- " << std::endl;

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
	Process *tail = *head, *current_node = new Process{pid, burst_time, arrival_time, priority, 0, 0, 0, 0, 0, false};

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
 * @brief swap nodes
 * @param current_node
 * @param compare_node
 */
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

/**
 * @brief
 * @param list node head
 * @param method ["pid", "sjf", "sjfp", "fcfs", "priority"]
 * @return Process
 */
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
				if (current_node->arrival_time > compare_node->arrival_time)
					swapNodes(current_node, compare_node);
				else if (current_node->arrival_time && current_node->burst_time > compare_node->burst_time)
					swapNodes(current_node, compare_node);
			}

			if (method.compare("pid") == 0)
				if (current_node->pid > compare_node->pid)
					swapNodes(current_node, compare_node);

			if (method.compare("sjfp") == 0)
				if (current_node->remaining_time < compare_node->remaining_time)
					swapNodes(current_node, compare_node);

			if (method.compare("fcfs") == 0)
				if (current_node->arrival_time > compare_node->arrival_time)
					swapNodes(current_node, compare_node);

			if (method.compare("priority") == 0)
			{
				if (current_node->arrival_time != compare_node->arrival_time)
				{
					if (current_node->arrival_time < compare_node->arrival_time)
						swapNodes(current_node, compare_node);
				}
				else
				{
					if (current_node->priority < compare_node->priority)
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

/**
 * @brief display results for first come first serve algorithm
 *
 * @return void
 */
void calculateFCFS()
{
	Process *current_node = sortLinkedList(head, "fcfs");
	float first_response = 0.0f, total_waiting_time = 0.0f;

	std::cout << " --------------- Scheduling Method: First Come First Served --------------- " << std::endl;
	std::cout << std::endl;

	std::cout << " Process Waitng times [ms]: " << std::endl;
	while (current_node != nullptr)
	{
		if (current_node->pid == 1)
			first_response = current_node->arrival_time;

		current_node->waiting_time = ((first_response + current_node->burst_time) - current_node->arrival_time) - current_node->burst_time;
		std::cout << " P" << current_node->pid << ": " << current_node->waiting_time << std::endl;

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
 * @brief display results for shortest job first algorithm
 *
 * @return void
 */
void calculateSJFNonPremptive()
{

	Process *current_node = sortLinkedList(head, "sjf");
	float total_waiting_time = 0.0f, elapsed_time = 0.0f;

	std::cout << " --------------- Scheduling Method: Shortest Job First ( NON-PREEMPTIVE ) --------------- " << std::endl;
	std::cout << std::endl;

	std::cout << " Process Waitng times [ms]: " << std::endl;
	while (current_node != nullptr)
	{
		current_node->waiting_time = elapsed_time - current_node->arrival_time;
		std::cout << " P" << current_node->pid << ": " << current_node->waiting_time << std::endl;

		total_waiting_time += current_node->waiting_time;
		elapsed_time += current_node->burst_time;
		current_node = current_node->next;
	}

	std::cout << " -------------------------------------------------------------------------- " << std::endl;
	std::cout << " > Average waiting time: " << total_waiting_time / TOTAL_PROCESS << "ms" << std::endl;
	std::cout << " -------------------------------------------------------------------------- " << std::endl;
}

/**
 * @brief display results for shortest job first algorithm (Preemptive)
 *
 * @return void
 */
void calculateSJFPremptive()
{
	Process *current_node = sortLinkedList(head, "sjfp");
	float total_waiting_time = 0.0f, elapsed_time = 0.0f;

	std::cout << " --------------- Scheduling Method: Shortest Job First ( PREEMPTIVE ) --------------- " << std::endl;
	std::cout << std::endl;
	std::cout << " Process Waitng times [ms]: " << std::endl;

	std::cout << " -------------------------------------------------------------------------- " << std::endl;
	std::cout << " > Average waiting time: " << total_waiting_time / TOTAL_PROCESS << "ms" << std::endl;
	std::cout << " -------------------------------------------------------------------------- " << std::endl;
}

/**
 * @brief display results for priority scheduling algorithm (Preemptive)
 *
 * @return void
 */
void calculatePriority()
{
	Process *current_node = sortLinkedList(head, "priority");
	float total_waiting_time = 0.0f, elapsed_time = 0.0f;

	std::cout << " --------------- Scheduling Method: Priority ( PREEMPTIVE ) --------------- " << std::endl;
	std::cout << std::endl;
	std::cout << " Process Waitng times [ms]: " << std::endl;

	std::cout << " -------------------------------------------------------------------------- " << std::endl;
	std::cout << " > Average waiting time: " << total_waiting_time / TOTAL_PROCESS << "ms" << std::endl;
	std::cout << " -------------------------------------------------------------------------- " << std::endl;
}