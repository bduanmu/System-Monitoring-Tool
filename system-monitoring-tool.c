#define _POSIX_C_SOURCE 199309L

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
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
        if (strcmp(argv[i], "--memory") == 0) arguments[2] = true;
        else if (strcmp(argv[i], "--cpu") == 0) arguments[3] = true;
        else if (strcmp(argv[i], "--cores") == 0) arguments[4] = true;
        else if (strncmp(argv[i], "--samples=", 10) == 0) arguments[0] = atoi(argv[i] + 10);
        else if (strncmp(argv[i], "--tdelay=", 9) == 0) arguments[1] = atoi(argv[i] + 9);
        else if (i == 1) arguments[0] = atoi(argv[i]);
        else if (i == 2) arguments[1] = atoi(argv[i]);
    }

    if (!arguments[2] && !arguments[3] && !arguments[4]) {
        arguments[2] = true;
        arguments[3] = true;
        arguments[4] = true;
    }
}

// Retrieves necessary data.
// info contains all required information in a list in the following order: [memory_usage, cpu_usage, num_cores, core_freq].
void retrieveData(char* info[4], bool show_memory, bool show_cpu, bool show_cores) {
    // Retrieving memory usage info.
    if (show_memory) {

    }

    // Retrieving CPU usage info.
    if (show_cpu) {

    }

    // Retrieving cores info.
    if (show_cores) {
        // Getting core max frequency data.
        FILE* fcore_freq = fopen("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq", "r");
        if (!fcore_freq) {
            fprintf(stderr, "Error reading file\n");
            exit(EXIT_FAILURE);
        }

        char core_freq[64];
        fgets(core_freq, 64, fcore_freq);
        info[3] = core_freq;

        fclose(fcore_freq);

        // Getting number of cores
        FILE* fcpu_info = fopen("/proc/cpuinfo", "r");
        if (!fcpu_info) {
            fprintf(stderr, "Error reading file\n");
            exit(EXIT_FAILURE);
        }

        char line[320];
        while (fgets(line, 320, fcpu_info)) {
            if (strncmp(line, "siblings", 8) == 0) {
                info[2] = line + 11; // Number appears at the 12th character.
                info[2][strlen(info[2]) - 1] = '\0'; // Removing the \n.
                break;
            }
        }

        fclose(fcpu_info);
    }
}

// Delays program by microseconds microseconds.
void delay(unsigned int microseconds) {
    struct timespec start, end;
    long elapsed;

    clock_gettime(CLOCK_MONOTONIC, &start);

    do {
        clock_gettime(CLOCK_MONOTONIC, &end);
        elapsed = (end.tv_sec - start.tv_sec) * 1000000L + (end.tv_nsec - start.tv_nsec) / 1000L;
    } while (elapsed < microseconds);
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
        char* info[4];
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
            printf("Num cores: %d, Max freq: %d\n", atoi(info[2]), atoi(info[3]));
        }

        printf("a");

        // Waits tdelay microseconds.
        delay(tdelay);
    }
    
    exit(EXIT_SUCCESS);
}

