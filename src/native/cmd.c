#include <stdio.h>
#include <stdlib.h>
#include <CoreFoundation/CoreFoundation.h>
#include "seq_lead_keys.h"
#include "parse_args.h"

static void on_partial_sequence(const char* sequence) {
    printf("SEQUENCE DETECTED: %s\n", sequence);
    fflush(stdout);
}

static void on_complete_sequence(const char* sequence) {
    printf("SEQUENCE COMPLETED: %s\n", sequence);
    fflush(stdout);
}

void print_usage(const char* program_name) {
    printf("Usage: %s [OPTIONS]\n", program_name);
    printf("Options:\n");
    printf("  -c, --command <sequence>  Add a command sequence to listen for\n");
    printf("  -h, --help               Show this help message\n");
    printf("\nExample:\n");
    printf("  %s --command abc --command xyz\n", program_name);
    printf("  %s -c abc -c xyz\n", program_name);
}

int main(int argc, char *argv[]) {
    ParsedArgs args = parse_arguments(argc, argv);

    if (args.help_requested) {
        print_usage(argv[0]);
        free_parsed_args(&args);
        return 0;
    }

    if (args.command_count == 0) {
        fprintf(stderr, "Error: No command sequences provided\n");
        print_usage(argv[0]);
        free_parsed_args(&args);
        return 1;
    }

    // Initialize the library
    if (!seq_lead_keys_init()) {
        fprintf(stderr, "Failed to initialize key listener\n");
        free_parsed_args(&args);
        return 1;
    }

    // Set up callbacks
    seq_lead_keys_set_partial_callback(on_partial_sequence);
    seq_lead_keys_set_complete_callback(on_complete_sequence);

    // Add commands to the tree
    for (int i = 0; i < args.command_count; i++) {
        if (!seq_lead_keys_add_command(args.commands[i])) {
            fprintf(stderr, "Failed to add command: %s\n", args.commands[i]);
            seq_lead_keys_cleanup();
            free_parsed_args(&args);
            return 1;
        }
    }

    // Start listening
    if (!seq_lead_keys_start()) {
        fprintf(stderr, "Error: Failed to start key listener. Check Accessibility permissions.\n");
        seq_lead_keys_cleanup();
        free_parsed_args(&args);
        return 1;
    }

    printf("Listening for command sequences...\n");
    CFRunLoopRun();

    // Cleanup
    seq_lead_keys_cleanup();
    free_parsed_args(&args);
    return 0;
} 