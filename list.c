#include "list.h"

// linked list implementation
// every node has trie-map instance for each doc in directory

listNode *CreateList(listNode **head)		//create head for list
{
	*head = (listNode*)malloc(sizeof(struct list));
	(*head)->name = NULL;
	(*head)->next = NULL;
	return *head;
}

void insert(listNode **head,char *name,int lines)
{
	if (*head == NULL)
	{
		*head = CreateList(head);
	}
	listNode *cur = *head;

	listNode *n = (listNode*)malloc(sizeof(struct list));
	n->name = malloc(sizeof(char)*(strlen(name)+1));
	strcpy(n->name, name);
	CreateTrie(&(n->trie));
	n->lines = lines;
	n->map = malloc(sizeof(char*)*lines);
	n->next = NULL;
		
	while (cur->next)
		cur = cur->next;
	cur->next = n;
}

void print(listNode **head)
{
	listNode *cur = *head;
	if (cur->next == NULL)
		printf("Empty list\n");
	else
	{
		while (cur->next !=NULL)
		{
			cur = cur->next;
			printf("%s\n",cur->name);
			// for (int i=0;i<cur->lines;i++)
			// 	printf("%s\n",cur->map[i]);
		}
	}
}

void length(listNode **head)
{
	int len = 0;
	listNode *cur = *head;
	while (cur !=NULL)
	{
		len++;
		cur = cur->next;
	}
	printf("Length %d\n",len);
}

void FreeList(listNode **head)
{
	listNode *cur = *head;
	listNode *temp;
	if (*head != NULL)
	{
		free((*head)->name);
		while (cur->next != NULL)
		{
			temp = cur->next;
			cur->next = temp->next;
			free(temp);
			if (cur == NULL)
				break;
		}
	}
}


//for /search 

// void insert_search(list_t **head,int number_of_times,int word_from)
// {
// 	list_t *n;

// 	if (*head == NULL)											//new code
// 	{
// 		*head = (list_t*)malloc(sizeof(struct list_search));
// 		(*head)->number_of_times = -1;
// 		(*head)->word_from = -1;
// 		(*head)->next = NULL;
// 	}
// 	list_t *cur = *head;


// 	if (cur->next == NULL)
// 	{
// 		n = (list_t*)malloc(sizeof(struct list_search));
// 		n->number_of_times = number_of_times;
// 		n->word_from = word_from;
// 		n->next = cur->next;
// 		cur->next = n;

// 	}
// 	else
// 	{
// 		while (cur->next)
// 		{
// 			cur = cur->next;
// 		}
// 		cur->next = (list_t*)malloc(sizeof(struct list_search));
// 		cur = cur->next;
// 		cur->number_of_times = number_of_times;
// 		cur->word_from = word_from;
// 		cur->next = NULL;
	
// 	}

// }

// void print_search(list_t **head)
// {
// 	list_t *cur = *head;
// 	if (cur->next == NULL)
// 		printf("Empty list\n");
// 	else
// 	{
// 		while (cur->next !=NULL)
// 		{
// 			cur = cur->next;
// 			printf("number_of_times %d\n\n",cur->number_of_times);
// 		}
// 	}
// }

// void FreeList_search(list_t **head)
// {
// 	list_t *cur = *head;
// 	list_t *temp;
// 	if (*head != NULL)
// 	{
// 		while (cur->next != NULL)
// 		{
// 			temp = cur->next;
// 			cur->next = temp->next;
// 			free(temp);
// 			if (cur == NULL)	//never in here probably
// 			{
// 				free(cur);
// 				break;
// 			}
// 		}
// 	}
// }