#include <stdlib.h>
#include <string.h>
#include "command_tree.h"

CommandNode* create_command_node(char character) {
    CommandNode* node = (CommandNode*)malloc(sizeof(CommandNode));
    if (node) {
        node->character = character;
        node->child_count = 0;
        node->is_terminal = false;
        memset(node->children, 0, sizeof(node->children));
    }
    return node;
}

void add_child(CommandNode* parent, CommandNode* child) {
    if (parent && child && parent->child_count < MAX_CHILDREN) {
        parent->children[parent->child_count++] = child;
    }
}

void add_command_sequence(CommandNode* root, const char* sequence) {
    if (!root || !sequence) return;
    
    CommandNode* current = root;
    for (int i = 0; sequence[i] != '\0'; i++) {
        CommandNode* child = find_child(current, sequence[i]);
        
        if (!child) {
            child = create_command_node(sequence[i]);
            add_child(current, child);
        }
        
        current = child;
    }
    
    current->is_terminal = true;
}

CommandNode* find_child(CommandNode* node, char character) {
    if (!node) return NULL;
    
    for (int i = 0; i < node->child_count; i++) {
        if (node->children[i]->character == character) {
            return node->children[i];
        }
    }
    
    return NULL;
}

void free_command_tree(CommandNode* root) {
    if (!root) return;
    
    for (int i = 0; i < root->child_count; i++) {
        free_command_tree(root->children[i]);
    }
    
    free(root);
} 