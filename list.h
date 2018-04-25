#ifndef _LISTH_
#define _LISTH_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trie.h"
#include "stack.h"

typedef struct trie trieNode_t;

typedef struct list 		//for posting list
{
	char *name;
	char **map;
	int lines;
	int *offset_array;
	int max_chars;
	struct list *next;
}listNode;

typedef struct Line_info 			// holds info about line and offset , used in posting list
{
	// int line;
	long offset;
	struct Line_info *next;
}line_info;

typedef struct Trie_list 			// posting list for trie , holds diffent info about keyword.
{
	char *name;
	int number_of_times;
	line_info *linfo;
	struct Trie_list *next;
}trie_list;

listNode *CreateList(listNode **);
void insert(listNode **,char *,int,int,int*,int);
void fill_trie(listNode **,trieNode_t **);
void print(listNode **);
void length(listNode **);
void FreeList(listNode **);


// struct for line info (offset-line)
void insert_lineInfo(line_info **,int , long );
void print_lineInfo(line_info **);
int linfo_length(line_info **);
void Free_lineInfo(line_info **);

//struct trie_list
trie_list *Create_Plist(trie_list **);
void insert_to_plist(trie_list **, char *,int , long);
int length_plist(trie_list **);
void print_plist(trie_list **);
void Free_plist(trie_list **);

#endif