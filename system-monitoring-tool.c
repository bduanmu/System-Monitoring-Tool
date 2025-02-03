#define _POSIX_C_SOURCE 199309L

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <sys/sysinfo.h>

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
void retrieveCoresData(char* info[2]) {
    // Getting core max frequency data.
    FILE* fcore_freq = fopen("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq", "r");
    if (!fcore_freq) {
        fprintf(stderr, "Error reading file\n");
        exit(EXIT_FAILURE);
    }

    char core_freq[64];
    fgets(core_freq, 64, fcore_freq);
    info[1] = core_freq;

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
            info[0] = line + 11; // Number appears at the 12th character.
            info[0][strlen(info[0]) - 1] = '\0'; // Removing the \n.
            break;
        }
    }

    fclose(fcpuinfo);
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

            printf("%lld", current_cpu_usage[0] - previous_cpu_usage[0]);
        }

        printf("a");

        // Waits tdelay microseconds.
        delay(tdelay);
    }

    
    // cores_info contains the number of cores and the maximum frequency in that order.
    char* cores_info[2];
    if (show_cores) retrieveCoresData(cores_info);
    if (show_cores) {
        // Print cores and frequency.
        printf("Num cores: %d, Max freq: %d\n", atoi(cores_info[0]), atoi(cores_info[1]));
    }
    
    exit(EXIT_SUCCESS);
}

