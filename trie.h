#ifndef _TRIEH_
#define _TRIEH_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "stack.h"

typedef struct Trie_list trie_list;

typedef struct trie{
	char key;
	struct trie* children;
	struct trie* neighbor;
	trie_list* plist;
	short endofword;
}trieNode_t;

void CreateTrie(trieNode_t **);
trieNode_t * CreateTrieNode(char);
void AddNode(trieNode_t **,char*,int,char*,int);
void printNode(trieNode_t **,char *);
void find_word(trieNode_t **,char *,char **,int*);
// void df(trieNode_t **);
void FreeTrie(trieNode_t **);

#endif