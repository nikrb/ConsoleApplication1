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
    newPair->size = (unsigned int)strlen(val);
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

void freeKeyValueLinkedList(KVPAIR *head) {
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

/****************************************************************************
* 
* Assignment code
* 
****************************************************************************/

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
    char* ntstring = (char*)malloc(size+1);
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


/****************************************************************************
*
* End of Assignment code
*
****************************************************************************/


int main() {
    // read some data from a csv file
    char filename[] = "key_value_pairs.txt";
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open the file");
        return EXIT_FAILURE;
    }
    // populate a linked list with the data
    KVPAIR *head = NULL;
    long key;
    char val[256];

    while (fscanf(file, "%ld,%255[^\n]", &key, val) == 2) {
        insertKeyValuePair(&head, key, val);
    }

    fclose(file);

    // Print the key-value pairs - check we have it correct
    printKeyValuePairs(head);

    // create the memory buffer with the data that will be passed to the assignment
    size_t buffer_size;
    char* buffer = packPairsIntoBuffer(head, &buffer_size);

    // check we created the buffer correctly
    printBufferContents(buffer, buffer_size);

    // free our dummy link list before proceeding with assignment code
    freeKeyValueLinkedList(head);

    /**********************************************************************
    * 
    * now do the assignment code, create a linked list from the memory buffer
    * 
    * ********************************************************************/
    KVPAIR* linked_list = deserialize(buffer, buffer_size);

    // check our linked list
    printKeyValuePairs(linked_list);

    // test lookup
    char* result = lookup(linked_list, 2);
    if (result != NULL) {
        printf("key 2 value: [%s]\n", result);
        // lookup allocates memory to create a null terminated copy of string value
        free(result);
    }
    // test lookup 2 - we declare a char buffer for this version
    //                  and we don't have to free any memory
    char str[32];
    result = lookup2(linked_list, 2, str, 32);
    if (result != NULL) {
        printf("key 2 value: [%s]\n", result);
    }
    // check truncation works correctly for lookup2
    char str2[3];
    result = lookup2(linked_list, 2, str2, 3);
    if (result != NULL) {
        printf("key 2 value: [%s]\n", result);
    }

    // test the delete function
    delete(&linked_list, 2);
    // check we removed key 2
    printKeyValuePairs(linked_list);

    // Free the memory
    freeKeyValueLinkedList(linked_list);
    free(buffer);

    return EXIT_SUCCESS;
}
