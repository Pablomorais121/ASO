#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

/*
* Hecho por Pablo Morais Alvarez
*/

void funcion();


void main(){

	char cmd[25];
	
	printf("Memoria antes de la ejecucion de la funcion \n");
	sprintf(cmd, "cat /proc/%d/maps",getpid());
	system(cmd);
	printf("\n\n");
	
	
	funcion();
	
	
	printf("Memoria despues de la ejecucion de la funcion\n");
	sprintf(cmd, "cat /proc/%d/maps",getpid());
	system(cmd);
	printf("\n\n");

}


void funcion(){
	
	char vector[14000];
	
	printf("Memoria despues de la ejecucion de la funcion\n");
	char cmd[25];
	sprintf(cmd, "cat /proc/%d/maps",getpid());
	system(cmd);
	printf("Variable/función   Dirección              Tamaño\n");
	printf("       	  vector   %p   %12ld\n", &vector, sizeof(vector));

}

