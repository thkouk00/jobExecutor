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
	// node->plist = NULL;
	node->endofword = 0;
	return node;
}

// add node to trie
// every node has only one child and one neighbor
// always start from root
void AddNode(trieNode_t **root,char *key,int id)
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
	if (!strncmp(buffer, str,strlen(str)) && tempNode->endofword)
	{
		finish = 1;
		// listNode *l = tempNode->plist;
		// if (l != NULL)
		// {
		// 	printf("NAME is %s\n",l->name);
		// }
	}
	else
	{
		finish = 0;
		printf("%s not found\n",str);
	}
	if (finish)
		printf("PRINT : %s\n",buffer);


	free(buffer);
}

// find word returns posting list for a given word if exists , else returns NULL
// same implementation as printNode
// listNode *find_word(trieNode_t **root,char *key)
// {
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
// 	if (!flag && !finish)
// 	{
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
// 			free(buffer);
// 			return tempNode->plist;
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