#ifndef PARSE_ARGS_H
#define PARSE_ARGS_H

#include <stdbool.h>

// Structure to hold parsed command line arguments
typedef struct {
    char** commands;
    int command_count;
    bool help_requested;
} ParsedArgs;

// Parse command line arguments
ParsedArgs parse_arguments(int argc, char *argv[]);

// Free resources used by ParsedArgs
void free_parsed_args(ParsedArgs* args);

#endif // PARSE_ARGS_H 