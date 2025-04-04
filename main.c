#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "application/domain/models.h"
#include "memory/stats.h"

#define INPUT_FILENAME "input.txt"
#define OUTPUT_FILENAME "output.txt"
#define MEMSTAT_FILENAME "memstat.txt"

#define MAX_EXPRESSION_LENGTH 256

void write_chars(const char* chars) {
    FILE* file = fopen(OUTPUT_FILENAME, "a");
    fprintf(file, "%s", chars);
    fclose(file);
}

int main(void) {
    FILE* input_file = fopen(INPUT_FILENAME, "r");
    if (input_file == NULL)
        return EXIT_FAILURE;

    FILE* output_file = fopen(OUTPUT_FILENAME, "w");
    if (output_file == NULL) {
        fclose(input_file);
        return EXIT_FAILURE;
    }

    fclose(output_file);

    char command[MAX_EXPRESSION_LENGTH];
    while (fgets(command, sizeof(command), input_file)) {
        if (strlen(command) == 0)
            continue;

        command[strcspn(command, "\n")] = '\0';
        execute_command(command, write_chars);
    }

    fclose(input_file);
    free_current_tree();

    FILE* memstat_file = fopen(MEMSTAT_FILENAME, "w");
    DataStats memstat;
    get_memstat(&memstat);
    fprintf(memstat_file, "malloc:%lu\ncalloc:%lu\nrealloc:%lu\nfree:%lu",
        memstat.malloc_state, memstat.calloc_state, memstat.realloc_state, memstat.free_count);

    fclose(memstat_file);
    return 0;
}
