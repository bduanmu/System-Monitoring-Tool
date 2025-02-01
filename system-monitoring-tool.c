#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

// Processes command line arguments.
// arguments holds the specifications of the program in a list in the following order: [samples, tdelay, memory, cpu, cores].
// memory, cpu, and cores are boolean values.
void processArguments(int argc, char* argv[], int arguments[5]) {
    arguments[0] = 20;
    arguments[1] = 500000;
    arguments[2] = false;
    arguments[3] = false;
    arguments[4] = false;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--memory") == 0) arguments[2] = 1;
        else if (strcmp(argv[i], "--cpu") == 0) arguments[3] = 1;
        else if (strcmp(argv[i], "--cores") == 0) arguments[4] = 1;
        else if (strncmp(argv[i], "--samples=", 10) == 0) arguments[0] = atoi(argv[i] + 10);
        else if (strncmp(argv[i], "--tdelay=", 9) == 0) arguments[1] = atoi(argv[i] + 9);
        else if (i == 1) arguments[0] = atoi(argv[i]);
        else if (i == 2) arguments[1] = atoi(argv[i]);
    }
}

// Retrieves necessary data.
void retrieveData(double info[4], bool show_memory, bool show_cpu, bool show_cores) {
    info[0] = -1;
    info[1] = -1;
    info[2] = -1;
    info[3] = -1;

    // Retrieving memory info
    if (show_memory) {

    }

    if (show_cpu) {

    }

    if (show_cores) {

    }
}

int main(int argc, char* argv[]) {
    // Disable output buffering.
    setbuf(stdout, NULL);

    // arguments holds the specifications of the program in a list in the following order: [samples, tdelay, memory, cpu, cores].
    // memory, cpu, and cores are boolean values.
    int arguments[5];
    processArguments(argc, argv, arguments);
    int samples = arguments[0];
    int tdelay = arguments[1];
    int show_memory = arguments[2];
    int show_cpu = arguments[3];
    int show_cores = arguments[4];

    // Main loop
    for (int i = 0; i < samples; i++) {
        // Retrieve data.
        // info contains all required information in a list in the following order: [memory_usage, cpu_usage, num_cores, core_freq].
        // Information that is not needed will be stored as -1. 
        double info[4];
        retrieveData(info, show_memory, show_cpu, show_cores);

        // Process data.

        // Output data.
        if (show_memory) {
            // Print memory utilization.
        }

        if (show_cpu) {
            // Print CPU utilization.
        }

        if (show_cores) {
            // Print cores and frequency.
        }

        printf("a");

        // Waits tdelay microseconds.
        usleep(tdelay);
    }
    
    return 0;
}

