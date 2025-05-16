#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse_args.h"

ParsedArgs parse_arguments(int argc, char *argv[]) {
    ParsedArgs args = {
        .commands = NULL,
        .command_count = 0,
        .help_requested = false
    };

    // Allocate initial array for commands
    args.commands = malloc(sizeof(char*) * argc);
    if (!args.commands) {
        fprintf(stderr, "Memory allocation failed\n");
        return args;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            args.help_requested = true;
            continue;
        }

        if (strcmp(argv[i], "--command") == 0 || strcmp(argv[i], "-c") == 0) {
            if (i + 1 < argc) {
                args.commands[args.command_count++] = strdup(argv[++i]);
            } else {
                fprintf(stderr, "Error: --command/-c requires a value\n");
            }
        }
    }

    return args;
}

void free_parsed_args(ParsedArgs* args) {
    if (args->commands) {
        for (int i = 0; i < args->command_count; i++) {
            free(args->commands[i]);
        }
        free(args->commands);
    }
} 