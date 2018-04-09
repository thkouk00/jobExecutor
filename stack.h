#ifndef _STACKH_
#define _STACKH_
#include "trie.h"

typedef  struct trie trieNode_t;

typedef struct stack
{
	trieNode_t *Tnode;
	struct stack *next;
}stackNode_t;

void push(stackNode_t **, trieNode_t *);
trieNode_t *pop(stackNode_t **);
void print_stack(stackNode_t **);

#endif