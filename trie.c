#include "trie.h"

void CreateTrie(trieNode_t **root)			//create root
{
	*root = CreateTrieNode('\n');
}

trieNode_t * CreateTrieNode(char key)		//initialize root
{
	trieNode_t *node = NULL;
	node = (trieNode_t *)malloc(sizeof(trieNode_t));

	if(node == NULL)
	{
		fprintf(stderr,"Malloc failed\n");
		return node;
	}
	node->key = key;
	node->children = NULL;
	node->neighbor = NULL;
	node->plist = NULL;
	return node;
}

// add node to trie
// every node has only one child and one neighbor
// always start from root
void AddNode(trieNode_t **root,char *key,int line_num, char *name,int offset)
{
	char *tmpstr = key;
	char *tmp1 ;
	trieNode_t *tempNode = NULL;

	if (*root == NULL)
	{
		fprintf(stderr, "Not initialized Trie!\n");
		exit(1);
	}
	//ignore html tags - project3
	if (*key == '<' || *key == '>' || !strncmp(key, "href=", 5))
		return ;
	tempNode = *root;
	while (*key != '\0')
	{
		if (tempNode->children == NULL)		//if child is NULL then create new Node
		{
			tempNode->children = CreateTrieNode(*key);	
			tempNode = tempNode->children;											 
		}
		else
		{
			tempNode = tempNode->children;			//if child exists , check if key is already in Node
			if (tempNode->key != *key)				// if key is different then check for neighbors
			{
				if (tempNode->neighbor == NULL)		// if there are not any neighbors create new Node 
				{
					tempNode->neighbor = CreateTrieNode(*key);
					tempNode = tempNode->neighbor;
				}
				else
				{
					int flag = 0;
					while (tempNode->neighbor != NULL)		//else if neighbor exists check for key
					{
						tempNode = tempNode->neighbor;
						if (tempNode->key == *key)
						{
							flag++;
							break;
						}
					}
					if (!flag)
					{
						tempNode->neighbor = CreateTrieNode(*key);
						tempNode = tempNode->neighbor;
					}
				}
			}
		}
		key++;
	}
	insert_to_plist(&(tempNode->plist), name, line_num, offset);
}

// print word if exists in trie , else print "not found"
void printNode(trieNode_t **root,char *key)
{
	int found = 0 , finish = 1, flag = 0;
	char *str = key;
	char *str1 ;
	char *buffer = malloc(sizeof(char)*(strlen(str)+1));
	if (buffer == NULL)
		printf("malloc error\n");

	trieNode_t *tempNode = NULL;

	if (*root == NULL)
	{
		fprintf(stderr, "Not initialized Trie!\n");
		exit(1);
	}
	
	tempNode = *root;
	int i;
	for (i=0;i<strlen(str);i++)
	{
		while (tempNode->children!=NULL)
		{
			tempNode = tempNode->children;
			if (tempNode->key == *key)
			{
				buffer[i] = tempNode->key;
				flag = 1;
				break;
			}
			else
			{
				while (tempNode->neighbor != NULL)
				{
					tempNode = tempNode->neighbor;
					if (tempNode->key == *key)
					{
						buffer[i] = tempNode->key;
						flag  = 1;
						found = 1;
						break;
					}
				}
				if (!flag)
					break;
				if (found)
					break;
			}
		}
		if (!flag)
			break;
		flag = 0;
		key++;

	}
	buffer[i] = '\0'; 	
	if (!strncmp(buffer, str,strlen(str)))
	{
		finish = 1;
		// trie_list *l = tempNode->plist;
		// while (l->next)
		// {
		// 	l = l->next;
		// 	// printf("NAME is %s and %d\n",l->name,l->number_of_times);
		// }
	}
	else
	{
		finish = 0;
		printf("%s not found\n",str);
	}
	if (finish)
		printf("PRINT : %s\n",buffer);

	// printf("%s\n",buffer);
	free(buffer);
}


void find_word(trieNode_t **root,char *key,char **name,int *number,int choice)
{
	int found = 0 , finish = 1, flag = 0;
	char *str = key;
	char *str1 ;
	char *buffer = malloc(sizeof(char)*(strlen(str)+1));
	if (buffer == NULL)
		printf("malloc error\n");

	trieNode_t *tempNode = NULL;

	if (*root == NULL)
	{
		fprintf(stderr, "Not initialized Trie!\n");
		exit(1);
	}
	
	tempNode = *root;
	int i;
	for (i=0;i<strlen(str);i++)
	{
		while (tempNode->children!=NULL)
		{
			tempNode = tempNode->children;
			if (tempNode->key == *key)
			{
				buffer[i] = tempNode->key;
				flag = 1;
				break;
			}
			else
			{
				while (tempNode->neighbor != NULL)
				{
					tempNode = tempNode->neighbor;
					if (tempNode->key == *key)
					{
						buffer[i] = tempNode->key;
						flag  = 1;
						found = 1;
						break;
					}
				}
				if (!flag)
					break;
				if (found)
					break;
			}
		}
		if (!flag)
			break;
		flag = 0;
		key++;

	}
	buffer[i] = '\0'; 	
	if (!strncmp(buffer, str,strlen(str)) && tempNode->plist!=NULL) 
	{
		char *name1=NULL;
		int count;
		trie_list *cur = tempNode->plist;
	
		if (choice == 3)			//for search
		{
			int buff_size = 100;
			char *result = malloc(sizeof(char)*buff_size);
			char *tmp_buff = malloc(sizeof(char)*30);
			memset(result, 0, sizeof(char)*buff_size);
			while (cur->next)
			{
				cur = cur->next;
				line_info *line_cur = cur->linfo;
				if (strlen(result)+strlen(cur->name) >= buff_size)
				{
					buff_size += buff_size;
					result = realloc(result, buff_size);
				}
				sprintf(result+strlen(result), "%s|",cur->name);
				while (line_cur->next)
				{
					line_cur = line_cur->next;
					sprintf(tmp_buff, "%ld|",line_cur->offset);
					
					if (strlen(result)+strlen(tmp_buff) >= buff_size)
					{
						buff_size += buff_size;
						result = realloc(result, buff_size);
					}	
					sprintf(result+strlen(result), "%ld|",line_cur->offset);
				}
				if (strlen(result)+3 >= buff_size)
				{
					buff_size +=3;
					result = realloc(result, buff_size);
				}
				sprintf(result+strlen(result), "$|");
			}
			*name = result;
			free(tmp_buff);
		}
		else
		{
			if (choice == 1)
			{
				//mincount
				count = 999999999;		
				while (cur->next)
				{
					cur = cur->next;
					if (cur->number_of_times < count)
					{
						if (name1 != NULL)
							free(name1);
						count = cur->number_of_times;
						name1 = malloc(sizeof(char)*(strlen(cur->name)+1));
						strcpy(name1, cur->name);
					}
				}
			}
			else if (choice == 0)
			{
				count = 0;
				while (cur->next)
				{
					cur = cur->next;
					if (cur->number_of_times > count)
					{
						if (name1 != NULL)
							free(name1);
						count = cur->number_of_times;
						name1 = malloc(sizeof(char)*(strlen(cur->name)+1));
						strcpy(name1, cur->name);
					}
				}
			}
			if (name1 != NULL)
			{
				*number = count;
				*name = malloc(sizeof(char)*(strlen(name1)+1));
				strcpy(*name, name1);
			}
			else
				name = NULL;	
		}
	}
	else
	{
		if (choice == 3)
			name = NULL;
	}
	free(buffer);
}

// free Trie structure
// using stack structure
void FreeTrie(trieNode_t **root)
{
	stackNode_t *head = NULL;
	trieNode_t *tempNode = NULL;
	trieNode_t *cur = NULL;
	tempNode = *root;
	trie_list *t_cur;
	trie_list *t_cur2;

	while (tempNode->children)
	{
		tempNode = tempNode->children;
		push(&head,tempNode);
	}
	while (head != NULL)
	{
		tempNode = pop(&head);
		if (tempNode == NULL)
			break;
		if(tempNode->neighbor != NULL)
		{
			push(&head, tempNode);
			cur = tempNode;
			tempNode = tempNode->neighbor;
			cur->neighbor = NULL;
			push(&head,tempNode);
			while (tempNode->children)
			{
				tempNode = tempNode->children;
				push(&head,tempNode);
			}
		}
		else
		{

			if (tempNode->plist != NULL)		//free posting list
			{	
				t_cur = tempNode->plist;
				while (t_cur->next)
				{
					t_cur2 = t_cur->next;
					t_cur->next = t_cur2->next;
					free(t_cur2->name);
					Free_lineInfo(&t_cur2->linfo);
					free(t_cur2->linfo);
					free(t_cur2);
				}
				free(tempNode->plist);
			}
			free(tempNode);
		}
	}
	free(head);
	free(*root);
}