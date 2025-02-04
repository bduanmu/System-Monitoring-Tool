#define _POSIX_C_SOURCE 199309L

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <sys/sysinfo.h>

// ESCape codes
#define CLEAR "\033[2J"
#define MOVE_CURSOR_TOP_LEFT "\033[H"
#define MOVE_CURSOR "\x1b[%d;%df"

// Formatting macros
#define CORES_SECTION_START_ROW 32
#define CPU_SECTION_START_ROW 17

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

// Retrieves memory data.
// memory_info stores [total_ram, free_ram, shared_ram, buffer_ram].
void retrieveMemoryData(long memory_info[4]) {
    // Retrieving memory usage info.
    struct sysinfo system_info;

    if (sysinfo(&system_info) != 0) {
        fprintf(stderr, "Error getting system info\n");
        exit(EXIT_FAILURE);
    }
    memory_info[0] = system_info.totalram * system_info.mem_unit;
    memory_info[1] = system_info.freeram * system_info.mem_unit;
    memory_info[2] = system_info.sharedram * system_info.mem_unit;
    memory_info[3] = system_info.bufferram * system_info.mem_unit;
}

// Retrieves CPU data.
// cpu_usage stores CPU times in the following order: [user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice].
void retrieveCPUData(long long cpu_usage[10]) {
    // Retrieving CPU times info.
    FILE* fcpu_usage = fopen("/proc/stat", "r");
    if (!fcpu_usage) {
        fprintf(stderr, "Error reading file\n");
        exit(EXIT_FAILURE);
    }

    fscanf(fcpu_usage, "cpu  %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld", 
        &cpu_usage[0], &cpu_usage[1], &cpu_usage[2], &cpu_usage[3], &cpu_usage[4], 
        &cpu_usage[5], &cpu_usage[6], &cpu_usage[7], &cpu_usage[8], &cpu_usage[9]);
    
    fclose(fcpu_usage);
}

// Retrieves cores data.
// info will contain the number of cores and the maximum frequency in that order.
void retrieveCoresData(long info[2]) {
    // Getting core max frequency data.
    FILE* fcore_freq = fopen("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq", "r");
    if (!fcore_freq) {
        fprintf(stderr, "Error reading file\n");
        exit(EXIT_FAILURE);
    }

    char core_freq[64];
    fgets(core_freq, 64, fcore_freq);
    info[1] = atoi(core_freq);

    fclose(fcore_freq);

    // Getting number of cores
    FILE* fcpuinfo = fopen("/proc/cpuinfo", "r");
    if (!fcpuinfo) {
        fprintf(stderr, "Error reading file\n");
        exit(EXIT_FAILURE);
    }

    char line[320];
    while (fgets(line, 320, fcpuinfo)) {
        if (strncmp(line, "siblings", 8) == 0) {
            char* num_cores = line + 11; // Number appears at the 12th character.
            num_cores[strlen(num_cores) - 1] = '\0'; // Removing the \n.

            info[0] = atoi(num_cores);
            break;
        }
    }

    fclose(fcpuinfo);
}

// Processes the CPU usage and returns the percent usage.
double processCPUUtilization(long long previous_cpu_usage[10], long long current_cpu_usage[10]) {
    long long total_time = 0;
    for (int i = 0; i < 10; i++) total_time += current_cpu_usage[i] - previous_cpu_usage[i];

    long long idle_time = current_cpu_usage[3] - previous_cpu_usage[3] + current_cpu_usage[4] - previous_cpu_usage[4];

    return 100.0 - ((100.0 * idle_time) / (1.0 * total_time));
}

// Prints the formatted CPU utilization graph.
void outputCPUUtilization(double precent_usage, int current_sample, int total_samples) {
    // Move cursor to start of CPU section.
    printf(MOVE_CURSOR, CPU_SECTION_START_ROW, 0);
    printf("v CPU  %.2f %%", precent_usage);

    // Printing the axes.
    printf(MOVE_CURSOR, CPU_SECTION_START_ROW + 1, 0);
    printf("  100%%");
    printf(MOVE_CURSOR, CPU_SECTION_START_ROW + 11, 0);
    printf("    0%% ");
    for (int i = 0; i < total_samples + 2; i++) printf("-");
    for (int i = 0; i < 10; i++) {
        printf(MOVE_CURSOR, CPU_SECTION_START_ROW + 1 + i, 8);
        printf("|");
    }

    // Printing the graph.
    printf(MOVE_CURSOR, CPU_SECTION_START_ROW + 11 - (int)ceil(precent_usage / 10.0), 9 + current_sample);
    printf(":");
}

// Prints the formatted core info.
void outputCores(long num_cores, long max_freq) {
    // Move cursor to start of cores section.
    printf(MOVE_CURSOR, CORES_SECTION_START_ROW, 0); 
    printf("v Number of Cores: %ld @ %.2f GHz", num_cores, max_freq / 1000000.0);

    // Print a core for each core.
    for (int i = 0; i < num_cores; i++) {
        printf(MOVE_CURSOR, CORES_SECTION_START_ROW + 1 + 3 * (i / 4), 7 * (i % 4) + 1);
        printf("  +---+");
        printf(MOVE_CURSOR, CORES_SECTION_START_ROW + 1 + 3 * (i / 4) + 1, 7 * (i % 4) + 1);
        printf("  |   |");
        printf(MOVE_CURSOR, CORES_SECTION_START_ROW + 1 + 3 * (i / 4) + 2, 7 * (i % 4) + 1);
        printf("  +---+");
    }

    // Print new line.
    printf("\n");
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

    // Output number of samples and the delay between each sample.
    printf(CLEAR);
    printf(MOVE_CURSOR_TOP_LEFT);
    printf("Nbr of samples: %d -- every %d microSecs (%.3f secs)", samples, tdelay, tdelay / 1000000.0);

    // memory_info stores [total_ram, free_ram, shared_ram, buffer_ram].
    long memory_info[4];

    // previous_cpu_usage and current_cpu_usage stores CPU times in the following order: 
    // [user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice].
    long long previous_cpu_usage[10];
    long long current_cpu_usage[10];
    if (show_cpu) {
        // Retrieve initial CPU data.
        retrieveCPUData(current_cpu_usage);

        // Wait tdelay microseconds.
        delay(tdelay);
    }

    // Main loop
    for (int i = 0; i < samples; i++) {
        // Retrieve data.
        if (show_memory) retrieveMemoryData(memory_info);
        if (show_cpu) {
            // Update previous CPU data and retrieve the new data.
            memcpy(previous_cpu_usage, current_cpu_usage, 10 * sizeof(current_cpu_usage[0]));
            retrieveCPUData(current_cpu_usage);
        }

        // Process data.

        // Output data.
        if (show_memory) {
            // Print memory utilization.
        }

        if (show_cpu) {
            // Print CPU utilization.
            outputCPUUtilization(processCPUUtilization(previous_cpu_usage, current_cpu_usage), i, samples);
        }

        // Waits tdelay microseconds.
        delay(tdelay);
    }

    
    if (show_cores) {
        // cores_info contains the number of cores and the maximum frequency in that order.
        long cores_info[2];
        retrieveCoresData(cores_info);

        // Output cores and frequency.
        outputCores(cores_info[0], cores_info[1]);
    }
    
    // Move cursor to bottom left.
    printf(MOVE_CURSOR, 999, 0);
    exit(EXIT_SUCCESS);
}

