#include "function.h"


int main(){
	List_country_cap list;
	Init_List(list);
	readFile_and_addList(list);
	//show(list);

	char *cap, country[100];
	printf("\nNhap ten nuoc: ");
	gets(country);
	cap = search(list, country);
	if (cap == NULL){
		printf("Khong tim thay du lieu.");
	}
	else{
		printf("Ten thu do: %s", cap);
	}
	
	printf("\n");
	return 0;
}