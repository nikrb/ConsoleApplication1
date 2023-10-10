#pragma once

typedef struct KVP {
    long key;
    unsigned int size;
    char* val;
    struct KVP* next;
} KVPAIR;


KVPAIR* createKeyValuePairLink(KVPAIR* head, long key, const char* val, unsigned int size);
KVPAIR* deserialize(const char* buffer, size_t bufferSize);
char* lookup(KVPAIR* head, long key);
char* lookup2(KVPAIR* head, long key, char* result, unsigned int max_size);
void delete(KVPAIR** head, long key);
