#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ApplicationServices/ApplicationServices.h>
#include "command_tree.h"
#include "parse_args.h"

#define DOUBLE_CMD_INTERVAL 2.0
#define MAX_COMMANDS 128
#define LEAD_KEY_COUNT 2
#define MAX_SEQUENCE_LENGTH 128

// state
static int cmd_count = 0;
static CFAbsoluteTime first_cmd_time = 0;
static bool prefix_active = false;
static bool waiting_for_sub = false;
static CommandNode* command_tree = NULL;
static CommandNode* current_node = NULL;
static char current_sequence[MAX_SEQUENCE_LENGTH];
static int sequence_length = 0;

// Reset all state variables to their initial values
void reset_state(void) {
    printf("RESET STATE\n");
    prefix_active = false;
    waiting_for_sub = false;
    cmd_count = 0;
    current_node = NULL;
    sequence_length = 0;
    current_sequence[0] = '\0';
}

// called when a partial sequence is detected
void notify_partial_sequence(void) {
    printf("SEQUENCE DETECTED: %s\n", current_sequence);
    fflush(stdout);
}

// called when a full sequence fires
void notify_completed_sequence(void) {
    printf("SEQUENCE COMPLETED: %s\n", current_sequence);
    fflush(stdout);
}

CGEventRef callback(CGEventType type, CGEventRef event) {
    CFAbsoluteTime now = CFAbsoluteTimeGetCurrent();
    if (type == kCGEventFlagsChanged) {
        CGEventFlags flags = CGEventGetFlags(event);
        bool cmdDown = (flags & kCGEventFlagMaskCommand) != 0;
        if (cmdDown) {
            if (cmd_count == 0) {
                first_cmd_time = now;
                cmd_count = 1;
            } else if (!prefix_active && (now - first_cmd_time) > DOUBLE_CMD_INTERVAL) {
                cmd_count = 0;
                reset_state();
            } else {
                cmd_count++;
                if (cmd_count == LEAD_KEY_COUNT) {
                    printf("LEAD SEQUENCE DETECTED\n");
                    prefix_active = true;
                    waiting_for_sub = true;
                    current_node = command_tree;
                    sequence_length = 0;
                    current_sequence[0] = '\0';
                }
            }
        }
    }
    else if (type == kCGEventKeyDown && prefix_active) {
        // grab the Unicode char
        UniChar buf[4];
        UniCharCount len = 0;
        CGEventKeyboardGetUnicodeString(event, 4, &len, buf);
        if (len == 0) {
            reset_state();
            return event;
        }
        char ch = (char)buf[0];

        if (current_node) {
            CommandNode* next = find_child(current_node, ch);
            if (next) {
                current_node = next;
                // Add character to current sequence
                if (sequence_length < MAX_SEQUENCE_LENGTH - 1) {
                    current_sequence[sequence_length++] = ch;
                    current_sequence[sequence_length] = '\0';
                    notify_partial_sequence();
                }
                
                if (current_node->is_terminal) {
                    // We found a complete command sequence
                    notify_completed_sequence();
                    reset_state();
                    return NULL;  // swallow the key
                }
                return NULL;  // swallow the key while building sequence
            }
        }
        reset_state();
    }

    return event;
}

// Wrapper function to match the required callback signature
static CGEventRef event_callback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon) {
    return callback(type, event);
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

    // Create command tree
    command_tree = create_command_node('\0');  // Root node
    if (!command_tree) {
        fprintf(stderr, "Failed to create command tree\n");
        free_parsed_args(&args);
        return 1;
    }

    // Add commands to the tree
    for (int i = 0; i < args.command_count; i++) {
        add_command_sequence(command_tree, args.commands[i]);
    }

    // install the event tap
    CGEventMask mask = CGEventMaskBit(kCGEventFlagsChanged)
                     | CGEventMaskBit(kCGEventKeyDown);
    CFMachPortRef tap = CGEventTapCreate(
        kCGHIDEventTap,
        kCGHeadInsertEventTap,
        kCGEventTapOptionDefault,
        mask,
        event_callback,
        NULL
    );
    if (!tap) {
        fprintf(stderr, "Error: Failed to create event tap. Check Accessibility permissions.\n");
        free_command_tree(command_tree);
        free_parsed_args(&args);
        return 1;
    }

    CFRunLoopSourceRef src =
      CFMachPortCreateRunLoopSource(NULL, tap, 0);
    CFRunLoopAddSource(CFRunLoopGetCurrent(),
                       src, kCFRunLoopCommonModes);
    CGEventTapEnable(tap, true);

    printf("Listening for command sequences...\n");
    CFRunLoopRun();

    // Cleanup
    free_command_tree(command_tree);
    free_parsed_args(&args);
    return 0;
}
