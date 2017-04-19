#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <malloc.h>
#include <string.h>

struct Node{
	char* country;
	char* capital;
	Node *next;
};

struct List_country_cap
{
	Node* Head;
	Node* Tail;
};

void Init_List(List_country_cap &L);
int is_Empty(List_country_cap L);
Node *make_Node(char *str);
void add(List_country_cap &L, char *str);
void readFile_and_addList(List_country_cap &L);
//void show(List_country_cap L);
char* search(List_country_cap L, char* country);