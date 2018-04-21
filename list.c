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

void insert(listNode **head,char *name,int lines,int max_chars,int *offset_array,int num_of_elements)
{
	if (*head == NULL)
	{
		*head = CreateList(head);
	}
	listNode *cur = *head;

	listNode *n = (listNode*)malloc(sizeof(struct list));
	n->name = malloc(sizeof(char)*(strlen(name)+2));
	strncpy(n->name, name,strlen(name)+1); //htan +1
	n->name[strlen(name)+2] = '\0';
	
	n->lines = lines;
	n->max_chars = max_chars;
	n->map = malloc(sizeof(char*)*lines);

	n->offset_array = malloc(sizeof(int)*(num_of_elements));
	
	memcpy(n->offset_array, offset_array, sizeof(int)*num_of_elements); 
	n->next = NULL;
	
	while (cur->next)
		cur = cur->next;
	cur->next = n;
}


void fill_trie(listNode **head,trieNode_t **root)
{
	listNode *cur = *head;
	char *str;
	char *str1;
	char delimiter[] = " \t\n";
	if (cur->next == NULL)
		printf("Empty list\n");
	else
	{
		while (cur->next)
		{
			cur = cur->next; 
			str = malloc(sizeof(char)*(cur->max_chars+1));
			for (int i =0;i<cur->lines;i++)
			{
				strncpy(str, cur->map[i], strlen(cur->map[i])+1);
				str1 = strtok(str, delimiter);
				while (str1!=NULL)
				{
					AddNode(root,str1,i,cur->name,cur->offset_array[i]);
					str1 = strtok(NULL, delimiter);
				}
			}
			free(str);
		}
	}
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

void FreeList(listNode **head)			// edw exw kanei free egw
{
	listNode *cur = *head;
	listNode *cur2;
	
	while (cur->next)
	{
		cur2 = cur->next;
		free(cur2->name);
		free(cur2->offset_array);
		for (int i=0;i<cur2->lines;i++)
			free(cur2->map[i]);
		free(cur2->map);
		cur->next = cur2->next;
		free(cur2);
	}
	free(*head);
}

// for line_info
void insert_lineInfo(line_info **head,int line, long offset)
{
	if (*head == NULL)
	{
		*head =(line_info*)malloc(sizeof(struct Line_info));
		(*head)->next = NULL;
	}

	int flag = 0;
	line_info *cur = *head;
	while (cur->next)					// insert new node at end of list
	{
		cur = cur->next;
		if (cur->line == line && cur->offset == offset)		//same word in same line
		{
			flag = 1;
			break;
		}
	}
	if (!flag)
	{
		line_info *n = (line_info*)malloc(sizeof(struct Line_info));
		n->line = line;
		n->offset = offset;
		n->next = NULL;
		cur->next = n;
	}
	// printf("AND line %d - ofs %ld\n", n->line,n->offset);
}

void print_lineInfo(line_info **head)
{
	line_info *cur = *head;
	while (cur->next)
	{
		cur = cur->next;
		printf("Line %d and offset %ld\n", cur->line,cur->offset);
	}
}

int linfo_length(line_info **head)
{
	line_info *cur = *head;
	int count = 0;
	while (cur->next)
	{
		cur = cur->next;
		count++;
	}
	return count;
}

void Free_lineInfo(line_info **head)	//thelei ftiaximo
{
	line_info *cur = *head;
	line_info *cur2;
	while (cur->next)
	{
		cur2 = cur->next;
		cur->next = cur2->next;
		free(cur2);
	}
}


// for trie_list
trie_list *Create_Plist(trie_list **head)		//create head for list
{
	*head = (trie_list*)malloc(sizeof(struct Trie_list));
	(*head)->name = NULL;
	(*head)->next = NULL;
	return *head;
}

void insert_to_plist(trie_list **head, char *name,int line,long offset)
{
	if (*head == NULL)
	{
		*head = Create_Plist(head);
	}
	trie_list *cur = *head;
	trie_list *cur1 = *head;
	int flag = 0;

	while (cur->next)
	{
		cur = cur->next;
		if (!strcmp(cur->name, name))
		{
			flag = 1;
			cur->number_of_times++;
			insert_lineInfo(&(cur->linfo), line, offset);
			break;
		}
	}
	if (!flag)
	{
		trie_list *temp = (trie_list*)malloc(sizeof(struct Trie_list));
		temp->name = malloc(sizeof(char)*(strlen(name)));
		strncpy(temp->name,name,strlen(name));
		temp->name[strlen(name)] = '\0';
		temp->linfo = NULL;
		temp->number_of_times = 1;
		insert_lineInfo(&(temp->linfo), line, offset);
		temp->next = cur1->next;
		cur1->next = temp;
	}

}

int length_plist(trie_list **head)
{
	int count = 0;
	trie_list *cur = *head;
	while (cur->next)
	{
		cur = cur->next;
		count++;
	}
	return count;
}

void print_plist(trie_list **);
// void Free_plist(trie_list **head)
// {
// 	trie_list *cur = *head;
// 	trie_list *cur2 = *head;
// 	while (cur->next)
// 	{
// 		cur2 = cur->next;
// 		free(cur2->name);
// 		line_info *temp = cur2->linfo;
// 		line_info *temp2;
// 		while (temp->next)
// 		{
// 			temp2 = temp->next;
// 			temp->next = temp2->next;
// 			free(temp2);
// 		}
// 		free(temp);
// 		cur->next = cur2->next;
// 		free(cur2);
// 	}
// }





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