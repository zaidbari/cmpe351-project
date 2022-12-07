/* -------------------------------- Libraries ------------------------------- */
#include <iostream>
#include <unistd.h>
#include <string.h>

/* ------------------------------- Namespaces ------------------------------- */
using namespace std;

/**
 * @brief Main entry point
 * @param argc Argument count (contains the number of arguments)
 * @param argv  Argument vector (contains argument's values)
 *
 * @return int EXIT_SUCCESS = 1
 */
int main(int argc, char *argv[])
{

	char *input_file_name, *output_file_name;
	for (int i = 1; i < argc; i++)
	{
		if (i + 1 != argc)
		{
			if (strcmp(argv[i], "-f") == 0)
			{
				input_file_name = argv[i + 1];
				i++;
			}
			if (strcmp(argv[i], "-o") == 0)
			{
				output_file_name = argv[i + 1];
				i++;
			}
		}
	}

	exit(EXIT_SUCCESS);
}
