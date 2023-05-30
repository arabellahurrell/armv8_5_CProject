//
// Created by Arabella Hurrell on 30/05/2023.
//

#include "symbol_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int hashMapSize = malloc(sizeof());
// Node structure for key-value pairs
typedef struct {
    char key[50];
    int value;
} Node;

// Hash map structure
typedef struct {
    Node* array[hashMapSize];
} HashMap;

// Hash function
int hash(char* key) {
    int sum = 0;
    int len = strlen(key);
    for (int i = 0; i < len; i++) {
        sum += key[i];
    }
    return sum % hashMapSize;
}

// Initialize the hash map
void initializeHashMap(HashMap* hashMap) {
    for (int i = 0; i < hashMapSize; i++) {
        hashMap->array[i] = NULL;
    }
}

// Insert key-value pair into the hash map
void insert(HashMap* hashMap, char* key, int value) {
    int index = hash(key);
    Node* newNode = (Node*)malloc(sizeof(Node));
    strcpy(newNode->key, key);
    newNode->value = value;
    hashMap->array[index] = newNode;
}

// Get the value associated with a key
int get(HashMap* hashMap, char* key) {
    int index = hash(key);
    Node* node = hashMap->array[index];
    if (node != NULL && strcmp(node->key, key) == 0) {
        return node->value;
    }
    return -1;  // Key not found
}

// Main function for testing the hash map
// int main() {
//     HashMap hashMap;
//     initializeHashMap(&hashMap);
//     return 0;
// }
