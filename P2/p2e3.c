#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
* Hecho por Pablo Morais Alvarez
*/

void main(){
	
	char cmd[25];
	
	printf("Memoria antes de crear el puntero\n");
	sprintf(cmd, "cat /proc/%d/maps",getpid());
	system(cmd);
	printf("\n\n");
	
	
	char *v;
	v = (char *)malloc(14000);
	
	printf("Memoria despues de crear el puntero\n");
	sprintf(cmd, "cat /proc/%d/maps",getpid());
	system(cmd);
	printf("Variable/función   Dirección              Tamaño\n");
	printf("       	       v   %p   %12ld\n", &v, sizeof(v));
	printf("\n\n");
	
	
	
	free(v);
	printf("Memoria despues de liberar el puntero\n");
	sprintf(cmd, "cat /proc/%d/maps",getpid());
	system(cmd);
	printf("\n\n");
	
}
