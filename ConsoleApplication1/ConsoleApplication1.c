#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct KVP {
    long key;
    unsigned int size;
    char *val;
    struct KVP *next;
} KVPAIR;

void insertKeyValuePair(KVPAIR **head, long key, const char *val) {
    KVPAIR *newPair = (KVPAIR *)malloc(sizeof(KVPAIR));
    if (newPair == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    newPair->key = key;
    newPair->size = strlen(val) + 1; // Include the null terminator
    newPair->val = (char *)malloc(newPair->size);
    if (newPair->val == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    if (memcpy(newPair->val, val, newPair->size) == NULL) {
        perror("memcpy failed\n");
        exit(EXIT_FAILURE);
    }
    newPair->next = *head;
    *head = newPair;
}

void printKeyValuePairs(KVPAIR *head) {
    KVPAIR *current = head;
    while (current != NULL) {
        printf("Key: %ld, Value: %s\n", current->key, current->val);
        current = current->next;
    }
}

void freeKeyValuePairs(KVPAIR *head) {
    while (head != NULL) {
        KVPAIR *temp = head;
        head = head->next;
        free(temp->val);
        free(temp);
    }
}

int main() {
    char filename[] = "key_value_pairs.txt";
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open the file");
        return EXIT_FAILURE;
    }

    KVPAIR *head = NULL;
    long key;
    char val[256];

    while (fscanf(file, "%ld,%255[^\n]", &key, val) == 2) {
        insertKeyValuePair(&head, key, val);
    }

    fclose(file);

    // Print the key-value pairs
    printKeyValuePairs(head);

    // Free the allocated memory
    freeKeyValuePairs(head);

    return EXIT_SUCCESS;
}
