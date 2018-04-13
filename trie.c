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
	printf("key is %c\n", key);
	node->key = key;
	// printf("NODE KEY is %c\n", node->key);
	node->children = NULL;
	node->neighbor = NULL;
	node->plist = NULL;
	node->endofword = 0;
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

	
	tempNode = *root;
	// printf("INSERTING %s\n", tmpstr);
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
	
	tempNode->endofword = 1;
	trie_list *cur = tempNode->plist;
	printf("KEY %s^\n", tmpstr);
	// printf("NAME IS %s\n", name);
	// insert_to_plist(&(tempNode->plist), name, line_num, offset);
	insert_to_plist(&(tempNode->plist), name, line_num, offset);
	
	// listNode *cur = tempNode->plist;
	// insert(&(tempNode->plist),id,tmpstr);		// posting list for word
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
				// printf("%c\n", tempNode->key);
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
						// printf("%c\n", tempNode->key);
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
	if (!strncmp(buffer, str,strlen(str)) && tempNode->endofword)
	{
		finish = 1;
		trie_list *l = tempNode->plist;
		while (l->next)
		{
			l = l->next;
			printf("NAME is %s and %d\n",l->name,l->number_of_times);
		}
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
///////////////////////////////////
void find_word(trieNode_t **root,char *key,char **name,int *number)
{
	int found = 0 , finish = 1, flag = 0;
	char *str = key;
	// str = malloc(sizeof(key));
	// strcpy(str, key);
	// if (str[strlen(str)-1] == '\n')
	// 	str[strlen(str)-1] = '\0';
	// printf("STR %s^\n", str);
	// str[strlen(key)] = '\0';
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
				// printf("%c\n", tempNode->key);
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
						// printf("%c\n", tempNode->key);
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
	// if (str[strlen(str)-1] == '\n')
		// str[strlen(str)] = '\0';
	
	printf("BUFFER %s\nSTR %s and %d\n", buffer,str,tempNode->endofword);
	if (!strncmp(buffer, str,strlen(str)) && tempNode->endofword)
	{
		finish = 1;
		printf("HEREE\n");
		// free(buffer);
		int count = 0;
		char *name1;
		trie_list *cur = tempNode->plist;
		while (cur->next)
		{
			cur = cur->next;
			printf("***%s %d\n", cur->name,cur->number_of_times);
			if (cur->number_of_times > count)
			{
				count = cur->number_of_times;
				name1 = malloc(sizeof(char)*(strlen(cur->name)+1));
				strcpy(name1, cur->name);
			}
		}
		printf("LALALA %s\n",name1);
		*number = count;
		*name = malloc(sizeof(char)*(strlen(name1)+1));
		strcpy(*name, name1);
		printf("NAME %s and num %d\n", *name,*number);
		// return name;
			// tempNode->plist;	//h lista
			// return name tempNode->plist; 		// trav
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




//////////////////////////////////
// find word returns posting list for a given word if exists , else returns NULL
// same implementation as printNode
// char* find_word(trieNode_t **root,char *key,char *name,int *number)
// {
// 	printf("ZDAROVA\n");
// 	int found = 0 , finish = 1 , flag = 0;
// 	char *str = key;
// 	char *str1 ;
// 	char *buffer = malloc(sizeof(char)*(strlen(str)+1));
// 	if (buffer == NULL)
// 		printf("malloc error\n");

// 	trieNode_t *tempNode = NULL;

// 	if (*root == NULL)
// 	{
// 		fprintf(stderr, "Not initialized Trie!\n");
// 		exit(1);
// 	}
	
// 	tempNode = *root;
// 	int i;
// 	for (i=0;i<strlen(str);i++)					//compare child nodes or neighbors for given key
// 	{
// 		while (tempNode->children!=NULL)
// 		{
// 			tempNode = tempNode->children;
// 			if (tempNode->key == *key)
// 			{
// 				buffer[i] = tempNode->key;
// 				printf("%c\n", buffer[i]);
// 				flag = 1;
// 				break;
// 			}
// 			else
// 			{
// 				found = 0;
// 				while (tempNode->neighbor != NULL)
// 				{
// 					tempNode = tempNode->neighbor;
// 					if (tempNode->key == *key)
// 					{
// 						buffer[i] = tempNode->key;
// 						printf("%c\n", buffer[i]);
// 						flag = 1;
// 						found = 1;
// 						break;
// 					}
// 				}
// 				if (!flag)
// 					break;
// 				if (found)
// 					break;
// 			}
// 		}
// 		if (!flag)
// 		{
// 			finish = 0;
// 			break;
// 		}
// 		flag = 0;
// 		key++;
// 	}
// 	buffer[i] = '\0'; 	
// 	printf("____%s\n", buffer);
// 	if (!flag && !finish)
// 	{
// 		printf("EDW\n");
// 		return NULL;
// 	}
// 	if (!strcmp(buffer, str) && tempNode->endofword)
// 	{
// 		if (tempNode->plist == NULL)
// 		{
// 			free(buffer);
// 			return NULL;
// 		}
// 		else
// 		{
// 			printf("HEREE\n");
// 			free(buffer);
// 			int count = 0;
// 			char *name1;
// 			trie_list *cur = tempNode->plist;
// 			while (cur->next)
// 			{
// 				cur = cur->next;
// 				if (cur->number_of_times > count)
// 				{
// 					count = cur->number_of_times;
// 					name1 = malloc(sizeof(char)*(strlen(cur->name)+1));
// 					strcpy(name, cur->name);
// 				}
// 			}
// 			*number = count;
// 			name = malloc(sizeof(char)*(strlen(name1)+1));
// 			printf("NAME %s and num %d\n", name,*number);
// 			return name;
// 			// tempNode->plist;	//h lista
// 			// return name tempNode->plist; 		// traverse list and find the doc
// 		}
// 	}
// 	else
// 	{
// 		finish = 0;
// 		free(buffer);
// 		return NULL;

// 	}
// }

// // df takes zero or one argument
// // zero arguments , df returns every word from index with the number of docs each word is in
// // one argument , search and find if exist the given word and print the number of docs as above
// void df(trieNode_t **root)
// {
// 	stackNode_t *head = NULL;
// 	trieNode_t *tempNode = NULL;
// 	trieNode_t *cur = NULL;
// 	listNode *plist = NULL;
// 	int times;
// 	tempNode = *root;
// 	if (tempNode->children == NULL)
// 		printf("NO DATA\n");
// 	else
// 	{
// 		tempNode = tempNode->children;
// 		while (tempNode)									//use stack to traverse Trie and find every word
// 		{
// 			if (tempNode->neighbor)
// 			{
// 				push(&head, tempNode->neighbor);
// 			}
// 			if (tempNode->plist)
// 			{
// 				times = 0;
// 				plist = tempNode->plist;
// 				while (plist->next)
// 				{
// 					plist = plist->next;
// 					times++;
// 				}
// 				printf("%s %d\n",tempNode->plist->name,times);
// 			}
// 			if (tempNode->children == NULL)
// 			{
// 				tempNode = pop(&head);
// 				if (tempNode == NULL)
// 					break;
// 			}
// 			else
// 				tempNode = tempNode->children;
// 		}
// 	}

// 	free(head);
// }


// free Trie structure
// using stack structure
void FreeTrie(trieNode_t **root)
{
	stackNode_t *head = NULL;
	trieNode_t *tempNode = NULL;
	trieNode_t *cur = NULL;
	tempNode = *root;

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
			// if (tempNode->plist != NULL)		//free posting list
			// {	
			// 	FreeList(&tempNode->plist);
			// 	free(tempNode->plist);
			// }
			free(tempNode);
		}
	}
	free(head);
	free(*root);
}