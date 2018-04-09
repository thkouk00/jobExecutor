#include "stack.h"

//stack implementation , used for freeing the Trie structure and /df

void push(stackNode_t **head, trieNode_t *Tnode)
{
	if (*head == NULL)
	{
		*head = (stackNode_t*)malloc(sizeof(struct stack));
		(*head)->Tnode = NULL;
		(*head)->next = NULL;
	}
	stackNode_t *tempNode;
	stackNode_t *cur;

	tempNode = *head;

	if (tempNode->next == NULL)
	{

		tempNode->next = (stackNode_t*)malloc(sizeof(struct stack));
		tempNode = tempNode->next;
		tempNode->Tnode = Tnode;
		tempNode->next = NULL;
	}
	else
	{
		cur = (stackNode_t*)malloc(sizeof(struct stack));
		cur->Tnode = Tnode;
		cur->next = tempNode->next;
		tempNode->next = cur;
	}

}

trieNode_t *pop(stackNode_t **head)
{
	stackNode_t *tempNode = NULL;
	stackNode_t *cur = NULL;
	trieNode_t *temp = NULL;
	tempNode = *head;

	if (tempNode->next == NULL)
	{
		return NULL;
	}
	else
	{
		cur = tempNode->next;
		tempNode->next = cur->next;
		temp = cur->Tnode;
		free(cur);
		return temp;
	}

}


void print_stack(stackNode_t **head)
{
	stackNode_t *tempNode = NULL;
	tempNode = *head;
	while (tempNode->next != NULL)
	{
		tempNode = tempNode->next;
		printf("key %c\n",tempNode->Tnode->key);
	}
}