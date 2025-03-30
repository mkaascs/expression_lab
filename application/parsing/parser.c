#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "../../memory/stats.h"

#define COMMANDS_COUNT 6

const char* command_strings[COMMANDS_COUNT] = {
    "parse", "load_prf", "load_pst", "save_prf", "save_pst", "eval"
};

char* tokenize(char* origin, const char symbol, char** save_pointer) {
    if (origin) *save_pointer = origin;
    else if (!*save_pointer || **save_pointer == '\0') return NULL;

    char* start = *save_pointer;
    while (*start && *start == symbol) start++;

    if (*start == '\0') return NULL;

    char* token = start;
    while (**save_pointer) {
        if (**save_pointer == symbol) {
            **save_pointer = '\0';
            (*save_pointer)++;

            while (**save_pointer && **save_pointer == symbol) (*save_pointer)++;

            return token;
        }

        (*save_pointer)++;
    }

    *save_pointer = NULL;
    return token;
}

int get_command_type(const char* command, CommandType* type) {
    const int command_length = strlen(command);
    char* command_copy = (char*)track_malloc(command_length + 1);
    strncpy(command_copy, command, command_length + 1);

    char* save_pointer;
    char* token = tokenize(command_copy, ' ', &save_pointer);

    for (int index = 0; index < COMMANDS_COUNT; index++) {
        if (strcmp(token, command_strings[index]) == 0) {
            *type = index;
            track_free(command_copy);
            return 1;
        }
    }

    track_free(command_copy);
    return 0;
}