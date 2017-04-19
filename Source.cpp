#include "function.h"

//Init list
void Init_List(List_country_cap &L){
	L.Head = NULL;
	L.Tail = NULL;

}

//check empty list
int is_Empty(List_country_cap L){
	return (L.Head == NULL);
}

//make node 
Node *make_Node(char *str){

	char *temp = (char *)malloc(strlen(str));
	temp = strtok(str, ",");
	Node *p = (Node *)malloc(sizeof (Node));
	p->country = temp;
	
	temp = strtok(NULL, "\n");
	p->capital = temp;
	//while (temp != NULL){
		
		/*printf("%s",temp);
		temp = strtok(NULL, "");
		printf("--");
		printf("%s", temp);
		temp = strtok(NULL, "");*/
	//}
	p->next = NULL;
	return p;
}

//add Node to List
void add(List_country_cap &L, char *str){
	Node *P = make_Node(str);
	if (L.Tail == NULL) // Danh sách đang rỗng
	{
		L.Tail = P;
		L.Head = L.Tail;
	}
	else
	{
		L.Tail->next = P;
		L.Tail = P;
	}

	/*printf("%s", L.Head->country);
	printf(" - %s", L.Tail->capital);*/
}

//read file
void readFile_and_addList(List_country_cap &L){

	char *r;
	FILE *f = fopen("list.txt", "rt");
	if (!f){
		printf("File not found.");
	}
	else{
		while (!feof(f)){
			r = (char *)malloc(255 * sizeof(char));
			fgets(r, 255, f);
			//printf("\n");
			//printf("%d", strlen(r));
			//printf("%s", r);
			add(L, r);
			
		}
		fclose(f);
		printf("\nFile is opened.\n");
	}
}

/*void show(List_country_cap L){
	Node *p = L.Head;
	while (p != NULL){
		printf("%s", p->country);
		printf(" - %s", p->capital);
		printf("\n");
		p = p->next;
	}
}*/


char* search(List_country_cap L, char* country){
	for (Node* p = L.Head; p != NULL; p = p->next)
	{
		if (strcmp(p->country, country) == 0){
			return p->capital;
		}
	}
	return NULL;
}