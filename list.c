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

void insert(listNode **head,char *name,int lines,int max_chars,int *offset_array)
{
	if (*head == NULL)
	{
		*head = CreateList(head);
	}
	listNode *cur = *head;

	listNode *n = (listNode*)malloc(sizeof(struct list));
	n->name = malloc(sizeof(char)*(strlen(name)+1));
	strncpy(n->name, name,strlen(name)+1);
	n->lines = lines;
	n->max_chars = max_chars;
	n->map = malloc(sizeof(char*)*lines);
	n->next = NULL;
	// printf("INSERT NAME ->%s and %s\n", n->name,name);
	while (cur->next)
		cur = cur->next;
	cur->next = n;
}


void fill_trie(listNode **head,trieNode_t **root,int path_num,int **offset_array)
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
			
			printf("NAME:\n%s\n",cur->name);
			for (int i =0;i<cur->lines;i++)
			{
				// printf("i:%d\n", i);
				strncpy(str, cur->map[i], strlen(cur->map[i])+1);
				// printf("STR->%s\n", cur->map[i]);
				str1 = strtok(str, delimiter);
				while (str1!=NULL)
				{
					
					//anti gia name kai path_num na dw mipos perasw to full path
					// AddNode(root,str1,i,cur->name,offset_array[path_num][i]);
					str1 = strtok(NULL, delimiter);
				}
			}
		}
		free(str);
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

// for line_info
void insert_lineInfo(line_info **head,int line, long offset)
{
	if (*head == NULL)
	{
		*head =(line_info*)malloc(sizeof(struct Line_info));
		(*head)->next = NULL;
	}

	line_info *cur = *head;
	while (cur->next)					// insert new node at end of list
	{
		cur = cur->next;
	}
	line_info *n = (line_info*)malloc(sizeof(struct Line_info));
	n->line = line;
	n->offset = offset;
	n->next = NULL;
	cur->next = n;
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

void Free_lineInfo(line_info **head)	//thelei ftiaximo
{
	line_info *cur = *head;
	line_info *tmp;
	while (cur->next)
	{
		cur = cur->next;
		tmp = cur->next;
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
	int flag = 0;
	while (cur->next)
	{
		cur = cur->next;
		if (!strcmp(cur->name, name))
		{
			// printf("YPARXEI PLIST\n");
			flag = 1;
			cur->number_of_times++;
			insert_lineInfo(&cur->linfo, line, offset);
			break;
		}
	}
	if (!flag)
	{
		trie_list *temp = (trie_list*)malloc(sizeof(struct Trie_list));
		temp->name = malloc(sizeof(char)*(strlen(name)+1));
		strcpy(temp->name,name);
		// printf("NAMEinPLIST %s\n", temp->name);
		// temp->path_num = path_num;
		temp->number_of_times = 1;
		insert_lineInfo(&(temp->linfo), line, offset);
		temp->next = NULL;
		cur->next = temp;
	}

}

void print_plist(trie_list **);
void Free_plist(trie_list **);





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