#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kv_store.h"

KVPAIR* createKeyValuePairLink(KVPAIR* head, long key, const char* val, unsigned int size) {
    KVPAIR* newPair = (KVPAIR*)malloc(sizeof(KVPAIR));
    if (newPair == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    newPair->key = key;
    newPair->size = size;
    newPair->val = (char*)malloc(size);
    if (newPair->val == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    if (memcpy(newPair->val, val, size) == NULL) {
        perror("Memory copy failed");
        exit(EXIT_FAILURE);
    }

    newPair->next = head;
    return newPair;
}

KVPAIR* deserialize(const char* buffer, size_t bufferSize) {
    KVPAIR* head = NULL;
    const char* ptr = buffer;

    while (ptr < buffer + bufferSize) {
        long key;
        unsigned int size;

        // Read the key (8 bytes)
        memcpy(&key, ptr, sizeof(long));
        ptr += sizeof(long);

        // Read the size (4 bytes)
        memcpy(&size, ptr, sizeof(unsigned int));
        ptr += sizeof(unsigned int);

        // Allocate memory for the value string
        char* value = (char*)malloc(size);
        if (value == NULL) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }

        // Read the value string
        memcpy(value, ptr, size);
        ptr += size;

        // Insert the key-value pair into the linked list
        head = createKeyValuePairLink(head, key, value, size);

        // Free the dynamically allocated memory for value
        free(value);
    }

    return head;
}

char* makeNullTerminatedStringCopy(const char* str, unsigned int size) {
    char* ntstring = (char*)malloc(size + 1);
    if (ntstring != NULL) {
        memcpy(ntstring, str, size);
        ntstring[size] = '\0';
    }
    return ntstring;
}

// NOTE return value is malloc'd so have to free it in calling function after use
char* lookup(KVPAIR* head, long key) {
    KVPAIR* current = head;
    while (current != NULL) {
        if (current->key == key) {
            return makeNullTerminatedStringCopy(current->val, current->size);
        }
        current = current->next;
    }
    return NULL;
}

// version 2 doesn't malloc any memory
char* lookup2(KVPAIR* head, long key, char* result, unsigned int max_size) {
    KVPAIR* current = head;
    while (current != NULL) {
        if (current->key == key) {
            unsigned int parsed_size = current->size;
            if (parsed_size >= max_size) {
                parsed_size = max_size - 1; // (null terminator)
            }
            memcpy(result, current->val, parsed_size);
            result[parsed_size] = '\0';
            return result;
        }
        current = current->next;
    }
    return NULL;
}

void delete(KVPAIR** head, long key) {
    KVPAIR* current = *head;
    KVPAIR* prev = NULL;

    // Traverse the list to find the key to delete
    while (current != NULL && current->key != key) {
        prev = current;
        current = current->next;
    }

    // If the key is found, remove the node
    if (current != NULL) {
        // If the node to delete is the head
        if (prev == NULL) {
            *head = current->next;
        }
        else {
            prev->next = current->next;
        }

        // Free memory allocated for the value and the node
        free(current->val);
        free(current);
    }
}
