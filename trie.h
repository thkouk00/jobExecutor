#ifndef _TRIEH_
#define _TRIEH_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include "list.h"
#include "stack.h"

typedef struct trie{
	char key;
	struct trie* children;
	struct trie* neighbor;
	// listNode* plist;
	short endofword;
}trieNode_t;

void CreateTrie(trieNode_t **);
trieNode_t * CreateTrieNode(char);
void AddNode(trieNode_t **,char*,int);
void printNode(trieNode_t **,char *);
// listNode *find_word(trieNode_t **,char *);
// void df(trieNode_t **);
void FreeTrie(trieNode_t **);

#endif