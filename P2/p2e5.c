#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>

/*
* Hecho por Pablo Morais Alvarez
*/

void *handler(void *ptr){
	sleep(2);
	int local = 10;
	printf("Variable/función   Dirección              Tamaño\n");
	printf("       	   local   %p   %12ld\n", &local, sizeof(local));
	printf("\n\n");
}


void main(){

	char cmd[25];

	pthread_t h1,h2;	
	
	printf("Memoria antes de crear el hilo\n");
	sprintf(cmd, "cat /proc/%d/maps",getpid());
	system(cmd);
	printf("\n\n");
	
	pthread_create(&h1, NULL, handler, NULL);
	pthread_create(&h2,NULL,handler,NULL);
	
	printf("Memoria despues de crear el hilo con una variable dentro\n");
	sprintf(cmd, "cat /proc/%d/maps",getpid());
	system(cmd);
	
	pthread_join(h1, NULL);
	pthread_join(h2, NULL);
	
	printf("Memoria despues de terminar el hilo\n");
	sprintf(cmd, "cat /proc/%d/maps",getpid());
	system(cmd);
	printf("\n\n");
	
	
}
