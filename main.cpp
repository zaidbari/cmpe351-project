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
#include <cstring>
#include <algorithm>
#include <queue>

/* -------------------------- Variable definitaions ------------------------- */
#define DELIMETER ':'
int TOTAL_PROCESS = 0;
bool shoudl_write_to_file = false;
char *output_file_name = NULL;

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

Process *head = nullptr; // PROCESSES linked list head
Process *sortLinkedList(Process *list, std::string method);
Process *getNextShortestProcess(Process *head, int currentTime, std::vector<int> completedProcesses);
Process *getNextHighestPriorityProcess(Process *head, int currentTime, std::vector<int> completedProcesses);

struct CompareArrivalTimeAndPriority
{
	bool operator()(Process const &p1, Process const &p2)
	{
		if (p1.arrival_time != p2.arrival_time)
		{
			return p1.arrival_time > p2.arrival_time;
		}
		else
		{
			return p1.priority > p2.priority;
		}
	}
};

// Comparison function to sort the queue by remaining time
struct CompareRemainingTime
{
	bool operator()(Process const &p1, Process const &p2)
	{
		return p1.remaining_time > p2.remaining_time;
	}
};

struct filenames
{
	char *output_file_name;
	char *input_file_name;
};

filenames getCommandLineArguments(int argc, char *argv[]);

/* ------------------------- function defininations ------------------------- */
void readInputFile(std::string input_file_name);

int displayMenu(bool premtive, int type, float TQ);
int displaySchedulingMenu();

void createProcess(Process **head, int pid, float arrival_time, float burst_time, int priority);

void calculateFCFS();
void calculateSJFNonPremptive();
void calculateSJFPremptive();
void calculatePriorityNonPreemptive();
void calculatePriorityPreemptive();
void calculateRoundRobin(float TQ);

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
	system("clear");

	/* -------------- get command line arguments and store results -------------- */
	filenames files = getCommandLineArguments(argc, argv);
	output_file_name = files.output_file_name;

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
				if (!isPreemptive)
					calculatePriorityNonPreemptive();
				else
					calculatePriorityPreemptive();
				break;
			case 4: // case for displaying round robin scheduling algorithm
				system("clear");
				calculateRoundRobin(time_quantum);
				break;
			}
			break;
		case 4:
			if (time_quantum == 0)
			{
				std::cout << " Input quantum time value: ";
				std::cin >> time_quantum;
			}
			shoudl_write_to_file = true;
			system("clear");
			calculateFCFS();
			calculateSJFNonPremptive();
			// calculateSJFPremptive();
			calculatePriorityNonPreemptive();
			calculateRoundRobin(time_quantum);
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

	// Check if both -f and -o arguments are present
	bool f_present = false;
	bool o_present = false;
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-f") == 0)
			f_present = true;
		if (strcmp(argv[i], "-o") == 0)
			o_present = true;
	}
	if (!f_present || !o_present)
	{
		std::cout << "Error: Please specify both -f and -o arguments." << std::endl;
		exit(1);
	}

	for (int i = 1; i < argc; i++)
		if (i + 1 != argc)
		{
			if (strcmp(argv[i], "-f") == 0)
				input_file_name = argv[i + 1];
			if (strcmp(argv[i], "-o") == 0)
				output_file_name = argv[i + 1];
			i++;
		}

	// Validate input file name
	FILE *input_file = fopen(input_file_name, "r");
	if (input_file == NULL)
	{
		system("clear");
		std::cout << " -------------------------------------------------------------------------- " << std::endl;
		std::cerr << " ERROR: "
							<< "\"" << input_file_name << "\""
							<< " doesn't exists or is un-reachable" << std::endl;
		std::cout << " -------------------------------------------------------------------------- " << std::endl;
		exit(1);
	}
	fclose(input_file);

	std::ifstream output_file(output_file_name);
	if (output_file.good())
	{
		// Output file exists, clear its content
		output_file.close();
		std::ofstream clear_output_file(output_file_name, std::ofstream::trunc);
		clear_output_file.close();
	}
	else
	{
		// Output file does not exist, create it
		std::ofstream create_output_file(output_file_name);
		create_output_file.close();
	}
	output_file.close();

	return filenames{output_file_name, input_file_name};
}

void writeToFile(std::string content)
{

	std::cout << content << std::endl;

	if (shoudl_write_to_file)
	{
		std::ofstream output_file;
		output_file.open(output_file_name, std::ios::out | std::ios::app);
		output_file << content << std::endl;
		output_file.close();
	}
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
	int number_of_process = 0; // Current Process number while inside the loop

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
 *	@param TQ quantam time value if type = 4 | Round-Robin Algorithm
 *
 *	@return void
 */
int displayMenu(bool premtive = false, int type = 0, float TQ = 0)
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

	std::cout << " [1]: Scheduling Method (" << scheduling_method << ") " << (TQ > 0 ? "| QT = " + std::to_string(TQ) : " ") << std::endl;
	std::cout << " [2]: Preemptive Mode (" << (premtive ? "YES" : "NO") << ") " << std::endl;
	std::cout << " [3]: Show results" << std::endl;
	std::cout << " [4]: End Program (Run all methods then display and write to file)" << std::endl;

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

	// "new" is same a "malloc" function that will allocate memory for the process structure and return it as a pointer
	Process *tail = *head, *current_node = new Process{pid, burst_time, arrival_time, priority, burst_time};

	if (*head == NULL)
		*head = current_node;
	else
	{
		while (tail->next != NULL)
			tail = tail->next;
		tail->next = current_node;
	}
}

float simulateFCFS()
{
	Process *current_node = sortLinkedList(head, "arrival_time");
	float first_response = 0.0f, total_waiting_time = 0.0f;
	while (current_node != nullptr)
	{
		if (current_node->pid == 1)
			first_response = current_node->arrival_time;

		current_node->waiting_time = ((first_response + current_node->burst_time) - current_node->arrival_time) - current_node->burst_time;

		writeToFile(" P" + std::to_string(current_node->pid) + ": " + std::to_string(current_node->waiting_time));

		total_waiting_time += current_node->waiting_time;
		first_response = first_response + current_node->burst_time;
		current_node = current_node->next;
	}

	return total_waiting_time;
}

/**
 * @brief display results for first come first serve algorithm
 *
 * @return void
 */
void calculateFCFS()
{

	writeToFile("--------------- Scheduling Method: First Come First Served ---------------");
	writeToFile(" Process waiting times [ms]:");

	float total_waiting_time = simulateFCFS();

	writeToFile("--------------------------------------------------------------------------");
	writeToFile(" > Average waiting time: " + std::to_string(total_waiting_time / TOTAL_PROCESS) + "ms");
	writeToFile("--------------------------------------------------------------------------");
}

float simulateSJFPreemptive()
{
	std::priority_queue<Process, std::vector<Process>, CompareRemainingTime> queue;
	Process *current_node = head;
	float total_waiting_time = 0.0f, current_time = 0.0f, elapsed_time = 0.0f;
	// Add all the processes to the queue
	while (current_node != NULL)
	{
		queue.push(*current_node);
		current_node = current_node->next;
	}

	// Run the algorithm until the queue is empty
	while (!queue.empty())
	{
		// Get the process with the shortest remaining time
		Process shortest = queue.top();
		queue.pop();

		// If the process has already completed, skip it
		if (shortest.completed)
			continue;
		else
		{
			// Update the waiting time and turnaround time for the process
			shortest.waiting_time += shortest.elapsed_time - shortest.arrival_time;
			shortest.turnaround_time = shortest.waiting_time + shortest.burst_time;
			shortest.completed = true;
			shortest.completion_time = shortest.elapsed_time + shortest.burst_time;
		}
		writeToFile(" P" + std::to_string(shortest.pid) + ": " + std::to_string(shortest.waiting_time));

		// Add the waiting time of the process to the total waiting time
		total_waiting_time += shortest.waiting_time;

		// Update the elapsed time for the other processes in the queue
		std::vector<Process> temp_queue;
		while (!queue.empty())
		{
			Process p = queue.top();
			queue.pop();
			p.elapsed_time += shortest.burst_time;
			temp_queue.push_back(p);
		}
		for (int i = 0; i < temp_queue.size(); i++)
		{
			queue.push(temp_queue[i]);
		}
	}

	return total_waiting_time;
}
/**
 * @brief display results for shortest job first algorithm (Non-Preemptive)
 *
 * @return void
 */
void calculateSJFPremptive()
{

	writeToFile("---------- Scheduling Method: Shortest Job First ( Preemptive ) ----------");
	writeToFile(" Process waiting times [ms]:");

	float total_waiting_time = simulateSJFPreemptive();

	writeToFile("--------------------------------------------------------------------------");
	writeToFile(" > Average waiting time: " + std::to_string(total_waiting_time / TOTAL_PROCESS) + "ms");
	writeToFile("--------------------------------------------------------------------------");
}

float simulateSJFNonPremptive()
{

	float total_waiting_time = 0.0f, elapsed_time = 0.0f;

	// Create a copy of the original process list
	Process *sorted = sortLinkedList(head, "arrival_time");
	Process *processList = sorted;

	// Initialize variables
	int currentTime = 0;
	Process *currentProcess = NULL;

	// Keep track of completed processes
	std::vector<int> completedProcesses;

	// Iterate until all processes have completed
	while (completedProcesses.size() < TOTAL_PROCESS)
	{
		// Get the next process with the shortest remaining burst time
		currentProcess = getNextShortestProcess(processList, currentTime, completedProcesses);

		// Update the waiting time for the current process
		currentProcess->waiting_time = currentTime - currentProcess->arrival_time;
		total_waiting_time += currentProcess->waiting_time;

		// Update the elapsed time for the current process
		currentProcess->elapsed_time = currentProcess->waiting_time + currentProcess->burst_time;

		// Update the current time
		currentTime += currentProcess->burst_time;

		// Mark the current process as completed
		completedProcesses.push_back(currentProcess->pid);
	}
	// Reset the processList pointer to the first node
	processList = sorted;
	Process *temp = processList;
	while (temp != NULL)
	{
		writeToFile(" P" + std::to_string(temp->pid) + ": " + std::to_string(temp->waiting_time));
		temp = temp->next;
	}

	return total_waiting_time;
}

/**
 * @brief display results for Shortest Job First algorithm (Preemptive)
 *
 * @return void
 */
void calculateSJFNonPremptive()
{
	writeToFile("-------- Scheduling Method: Shortest Job First ( Non-Preemptive ) --------");
	writeToFile(" Process waiting times [ms]:");

	float total_waiting_time = simulateSJFNonPremptive();

	writeToFile("--------------------------------------------------------------------------");
	writeToFile(" > Average waiting time: " + std::to_string(total_waiting_time / TOTAL_PROCESS) + "ms");
	writeToFile("--------------------------------------------------------------------------");
}

float simulateProrityNonPreemptive()
{
	float total_waiting_time = 0.0f, elapsed_time = 0.0f;

	// Create a copy of the original process list
	Process *sorted = sortLinkedList(head, "priority");
	Process *processList = sorted;

	// Initialize variables
	int currentTime = 0;
	Process *currentProcess = NULL;

	// Keep track of completed processes
	std::vector<int> completedProcesses;

	// Iterate until all processes have completed
	while (completedProcesses.size() < TOTAL_PROCESS)
	{
		// Get the next process with the shortest remaining burst time
		currentProcess = getNextShortestProcess(processList, currentTime, completedProcesses);

		// Update the waiting time for the current process
		currentProcess->waiting_time = currentTime - currentProcess->arrival_time;
		total_waiting_time += currentProcess->waiting_time;

		// Update the elapsed time for the current process
		currentProcess->elapsed_time = currentProcess->waiting_time + currentProcess->burst_time;

		// Update the current time
		currentTime += currentProcess->burst_time;

		// Mark the current process as completed
		completedProcesses.push_back(currentProcess->pid);
	}

	// Reset the processList pointer to the first node
	processList = sorted;
	Process *temp = processList;
	while (temp != NULL)
	{
		writeToFile(" P" + std::to_string(temp->pid) + ": " + std::to_string(temp->waiting_time));
		temp = temp->next;
	}

	return total_waiting_time;
}
/**
 * @brief display results for priority scheduling algorithm (Non-Preemptive)
 *
 * @return void
 */
void calculatePriorityNonPreemptive()
{
	writeToFile("------------- Scheduling Method: Priority ( Non-Preemptive ) -------------");
	writeToFile(" Process waiting times [ms]:");

	float total_waiting_time = simulateProrityNonPreemptive();

	writeToFile("--------------------------------------------------------------------------");
	writeToFile(" > Average waiting time: " + std::to_string(total_waiting_time / TOTAL_PROCESS) + "ms");
	writeToFile("--------------------------------------------------------------------------");
}

void calculatePriorityPreemptive()
{

	Process *sorted = sortLinkedList(head, "priority");

	writeToFile("---------- Scheduling Method: Prority ( Preemptive ) ----------");
	writeToFile(" Process waiting times [ms]:");

	float total_waiting_time = 0.0f;

	writeToFile("--------------------------------------------------------------------------");
	writeToFile(" > Average waiting time: " + std::to_string(total_waiting_time / TOTAL_PROCESS) + "ms");
	writeToFile("--------------------------------------------------------------------------");
}

/**
 * @brief display results for Round Robin scheduling algorithm
 *
 * @return void
 */
void calculateRoundRobin(float TQ)
{
	float total_waiting_time = 0.0f;
	writeToFile("------------ Scheduling Method: Round Robin ( TQ = " + std::to_string(TQ) + " ) ------------ ");
	writeToFile(" Process waiting times [ms]:");

	std::priority_queue<Process, std::vector<Process>, CompareArrivalTimeAndPriority> queue;
	Process *current_node = head;

	// Add all the processes to the queue
	while (current_node != NULL)
	{
		current_node->waiting_time = 0;
		queue.push(*current_node);
		current_node = current_node->next;
	}

	// Run the algorithm until the queue is empty
	while (!queue.empty())
	{
		// Get the next process in the queue
		Process p = queue.top();
		queue.pop();

		// If the process has already completed, skip it
		if (p.completed)
			continue;

		// Decrement the remaining time of the process by the time quantum
		p.remaining_time -= TQ;
		p.elapsed_time += TQ;

		// If the process has completed, update its waiting time and turnaround time
		if (p.remaining_time <= 0)
		{
			p.waiting_time = p.elapsed_time - p.arrival_time;
			p.turnaround_time = p.waiting_time + p.burst_time;
			p.completed = true;
			p.completion_time = p.elapsed_time;
			writeToFile(" P" + std::to_string(p.pid) + ": " + std::to_string(p.waiting_time));
		}
		// Otherwise, add the process back to the end of the queue
		else
		{
			queue.push(p);
		}

		// Add the waiting time of the process to the total waiting time
		total_waiting_time += p.waiting_time;
	}
	writeToFile("--------------------------------------------------------------------------");
	writeToFile(" > Average waiting time: " + std::to_string(total_waiting_time / TOTAL_PROCESS) + "ms");
	writeToFile("--------------------------------------------------------------------------");
}

/**
 * @brief get the higest priority node from process list
 * @param node
 * @param currentTime
 * @param completedProcesses
 * @return
 */
Process *getNextHighestPriorityProcess(Process *head, int currentTime, std::vector<int> completedProcesses)
{
	Process *highestPriorityProcess = NULL;
	Process *temp = head;

	while (temp != NULL)
	{
		// Skip completed processes
		if (std::find(completedProcesses.begin(), completedProcesses.end(), temp->pid) != completedProcesses.end())
		{
			temp = temp->next;
			continue;
		}

		// If the current process has the highest priority and has arrived at the current time, set it as the highest priority process
		if (highestPriorityProcess == NULL ||
				temp->priority > highestPriorityProcess->priority)
		{
			highestPriorityProcess = temp;
		}

		temp = temp->next;
	}

	return highestPriorityProcess;
}

/**
 * @brief get the shortest job node from process list
 * @param node
 * @param currentTime
 * @param completedProcesses
 * @return
 */
Process *getNextShortestProcess(Process *node, int currentTime, std::vector<int> completedProcesses)
{
	Process *shortestProcess = NULL;
	Process *temp = node;

	while (temp != NULL)
	{
		// Skip completed processes
		if (std::find(completedProcesses.begin(), completedProcesses.end(), temp->pid) != completedProcesses.end())
		{
			temp = temp->next;
			continue;
		}

		// If the current process has the shortest remaining burst time and has arrived at the current time, set it as the shortest process
		if (shortestProcess == NULL || (temp->burst_time - temp->elapsed_time < shortestProcess->burst_time - shortestProcess->elapsed_time && temp->arrival_time <= currentTime))
		{
			shortestProcess = temp;
		}

		temp = temp->next;
	}

	return shortestProcess;
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
 * @param method ["arrival_time"]
 * @return Process
 */
Process *sortLinkedList(Process *list, std::string method)
{
	// Check if the linked list is empty
	if (list == nullptr)
		return nullptr;

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
			// TODO: Add support for different priorities
			if (method.compare("arrival_time") == 0)
				if (current_node->arrival_time > compare_node->arrival_time)
					swapNodes(current_node, compare_node);

			if (method.compare("priority") == 0)
				if (current_node->priority < compare_node->priority)
					swapNodes(current_node, compare_node);

			if (method.compare("burst_time") == 0)
				if (current_node->burst_time > compare_node->burst_time)
					swapNodes(current_node, compare_node);

			compare_node = compare_node->next;
		}
		current_node = current_node->next;
	}

	// Return a pointer to the head of the sorted copy
	return copy_head;
}
