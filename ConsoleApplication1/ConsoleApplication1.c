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
    newPair->size = strlen(val);
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
        printf("Key: %ld, Value: %.*s\n", current->key, current->size, current->val);
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

/*
buffer format:
|key|size|value string|key|size|value string|
*/
char* packPairsIntoBuffer(KVPAIR* head, size_t* buffer_size) {
    size_t totalSize = 0;
    KVPAIR* current = head;
    while (current != NULL) {
        totalSize += sizeof(long);          // Size of key (8 bytes)
        totalSize += sizeof(unsigned int);  // Size of size (4 bytes)
        totalSize += current->size;         // Size of value string
        current = current->next;
    }
    *buffer_size = totalSize;
    char* buffer = (char*)malloc(totalSize);
    if (buffer == NULL) {
        perror("Buffer memory allocation failed");
        exit(EXIT_FAILURE);
    }
    char* ptr = buffer;
    current = head;
    while (current != NULL) {
        // Copy the key (8 bytes)
        memcpy(ptr, &(current->key), sizeof(long));
        ptr += sizeof(long);

        // Copy the size (4 bytes)
        memcpy(ptr, &(current->size), sizeof(unsigned int));
        ptr += sizeof(unsigned int);

        // Copy the value string
        memcpy(ptr, current->val, current->size);
        ptr += current->size;

        current = current->next;
    }
    return buffer;
}
void printBufferContents(char* buffer, size_t buffer_size) {
    char* ptr = buffer;  // Start at the beginning of the buffer

    while (ptr < buffer + buffer_size) {
        // Read the key (8 bytes)
        long key;
        memcpy(&key, ptr, sizeof(long));
        ptr += sizeof(long);

        // Read the size (4 bytes)
        unsigned int size;
        memcpy(&size, ptr, sizeof(unsigned int));
        ptr += sizeof(unsigned int);

        // Read the value string
        char* value = (char*)malloc(size);
        if (value == NULL) {
            perror("print buffer contents failed to malloc value");
            exit(EXIT_FAILURE);
        }
        memcpy(value, ptr, size);
        ptr += size;

        // Print the key, size, and value
        printf("Key: %ld, Size: %u, Value: %.*s\n", key, size, size, value);
        free(value);
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

    size_t buffer_size;
    char* buffer = packPairsIntoBuffer(head, &buffer_size);

    printBufferContents(buffer, buffer_size);

    // Free the allocated memory
    free(buffer);
    freeKeyValuePairs(head);

    return EXIT_SUCCESS;
}
