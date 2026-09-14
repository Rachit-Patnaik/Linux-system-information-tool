#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "system_info.h"
#include "cpu_info.h"
#include "mem_info.h"

static volatile int keep_running = 1;

void handle_sigint(int sig) {
    (void)sig;
    keep_running = 0;
}

int main(void) {
    signal(SIGINT, handle_sigint);

    SystemInfo sys;
    CpuInfo cpu;
    MemoryInfo mem;

    fetch_cpu_static_info(&cpu);
    calculate_cpu_usage();

    // Hide terminal cursor
    printf("\033[?25l");

    while (keep_running) {
        fetch_system_info(&sys);
        cpu.usage_percent = calculate_cpu_usage();
        fetch_mem_info(&mem);

        // Native Linux terminal clear
        if (system("clear") != 0) {
            printf("\033[H\033[J");
        }

        print_system_info(&sys);
        print_cpu_info(&cpu);
        print_mem_info(&mem);

        printf("\033[90mPress Ctrl+C to exit. Refresh rate: 1s\033[0m\n");
        fflush(stdout);

        sleep(1);
    }

    // Restore cursor visibility
    printf("\033[?25h\nExiting cleanly...\n");
    return 0;
}