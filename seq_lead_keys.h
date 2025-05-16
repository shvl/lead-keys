#ifndef SEQ_LEAD_KEYS_H
#define SEQ_LEAD_KEYS_H

#include <stdbool.h>
#include "command_tree.h"

// Initialize the key listener
bool seq_lead_keys_init(void);

// Start listening for key sequences
bool seq_lead_keys_start(void);

// Stop listening for key sequences
void seq_lead_keys_stop(void);

// Add a command sequence to listen for
bool seq_lead_keys_add_command(const char* sequence);

// Set callback for when a partial sequence is detected
void seq_lead_keys_set_partial_callback(void (*callback)(const char* sequence));

// Set callback for when a complete sequence is detected
void seq_lead_keys_set_complete_callback(void (*callback)(const char* sequence));

// Clean up resources
void seq_lead_keys_cleanup(void);

#endif // SEQ_LEAD_KEYS_H 