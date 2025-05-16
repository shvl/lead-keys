#ifndef COMMAND_TREE_H
#define COMMAND_TREE_H

#include <stdbool.h>

#define MAX_CHILDREN 128

typedef struct CommandNode {
    char character;
    struct CommandNode* children[MAX_CHILDREN];
    int child_count;
    bool is_terminal;  // indicates if this node represents a complete command
} CommandNode;

// Create a new command node
CommandNode* create_command_node(char character);

// Add a child node to a parent node
void add_child(CommandNode* parent, CommandNode* child);

// Add a command sequence to the tree
void add_command_sequence(CommandNode* root, const char* sequence);

// Find a child node with the given character
CommandNode* find_child(CommandNode* node, char character);

// Free the entire tree
void free_command_tree(CommandNode* root);

#endif // COMMAND_TREE_H 