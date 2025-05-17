#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ApplicationServices/ApplicationServices.h>
#include <pthread.h>
#include "seq_lead_keys.h"

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
static CFMachPortRef tap = NULL;
static CFRunLoopSourceRef src = NULL;

// Callbacks
static void (*lead_callback)(void) = NULL;
static void (*partial_callback)(const char*) = NULL;
static void (*complete_callback)(const char*) = NULL;

static pthread_t event_thread;
static bool thread_running = false;

// Reset all state variables to their initial values
static void reset_state(void) {
    prefix_active = false;
    waiting_for_sub = false;
    cmd_count = 0;
    current_node = NULL;
    sequence_length = 0;
    current_sequence[0] = '\0';
}

static CGEventRef callback(CGEventType type, CGEventRef event) {
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
                    prefix_active = true;
                    waiting_for_sub = true;
                    current_node = command_tree;
                    sequence_length = 0;
                    current_sequence[0] = '\0';
                    if (lead_callback) {
                        lead_callback();
                    }
                }
            }
        }
    }
    else if (type == kCGEventKeyDown) {
        if (prefix_active) {
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
                    if (sequence_length < MAX_SEQUENCE_LENGTH - 1) {
                        current_sequence[sequence_length++] = ch;
                        current_sequence[sequence_length] = '\0';
                        if (partial_callback) {
                            partial_callback(current_sequence);
                        }
                    }
                    
                    if (current_node->is_terminal) {
                        if (complete_callback) {
                            complete_callback(current_sequence);
                        }
                        reset_state();
                        return NULL;
                    }
                    return NULL;
                }
            }
            reset_state();
        } else {
            // reset state if not in prefix mode to not activate lead on cmd-c cmd 
            reset_state();
        }
    }

    return event;
}

static CGEventRef event_callback(CGEventTapProxy proxy __attribute__((unused)), 
                               CGEventType type, 
                               CGEventRef event, 
                               void *refcon __attribute__((unused))) {
    return callback(type, event);
}

static void* event_loop_thread() {
    CFRunLoopRun();
    return NULL;
}

bool seq_lead_keys_run_in_thread(void) {
    if (thread_running) {
        return false;
    }

    thread_running = true;
    int result = pthread_create(&event_thread, NULL, event_loop_thread, NULL);
    if (result != 0) {
        thread_running = false;
        return false;
    }

    return true;
}

bool seq_lead_keys_init(void) {
    command_tree = create_command_node('\0');
    return command_tree != NULL;
}

bool seq_lead_keys_start(void) {
    if (!command_tree) {
        return false;
    }

    CGEventMask mask = CGEventMaskBit(kCGEventFlagsChanged) | CGEventMaskBit(kCGEventKeyDown);
    tap = CGEventTapCreate(
        kCGHIDEventTap,
        kCGHeadInsertEventTap,
        kCGEventTapOptionDefault,
        mask,
        event_callback,
        NULL
    );

    if (!tap) {
        return false;
    }

    src = CFMachPortCreateRunLoopSource(NULL, tap, 0);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), src, kCFRunLoopCommonModes);
    CGEventTapEnable(tap, true);

    printf("Listening for command sequences...\n");
    CFRunLoopRun();

    return true;
}

void seq_lead_keys_stop(void) {
    if (tap) {
        CGEventTapEnable(tap, false);
    }
    if (src) {
        CFRunLoopRemoveSource(CFRunLoopGetCurrent(), src, kCFRunLoopCommonModes);
        CFRelease(src);
        src = NULL;
    }
    if (tap) {
        CFRelease(tap);
        tap = NULL;
    }
    
    if (thread_running) {
        CFRunLoopStop(CFRunLoopGetCurrent());
        pthread_join(event_thread, NULL);
        thread_running = false;
    }
}

bool seq_lead_keys_add_command(const char* sequence) {
    if (!command_tree || !sequence) {
        return false;
    }
    add_command_sequence(command_tree, sequence);
    return true;
}

void seq_lead_keys_set_lead_callback(void (*callback)(void)) {
    lead_callback = callback;
}

void seq_lead_keys_set_partial_callback(void (*callback)(const char* sequence)) {
    partial_callback = callback;
}

void seq_lead_keys_set_complete_callback(void (*callback)(const char* sequence)) {
    complete_callback = callback;
}

void seq_lead_keys_cleanup(void) {
    seq_lead_keys_stop();
    if (command_tree) {
        free_command_tree(command_tree);
        command_tree = NULL;
    }
} 