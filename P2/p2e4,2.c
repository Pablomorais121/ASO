#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

/*
* Hecho por Pablo Morais Alvarez
*/

char cmd[25];

int main(int argc, char *argv[]){
	
	printf("El valor del coseno es %f\n", cos(5));
	
	sprintf(cmd , "cat /proc/%d/maps", getpid ());

	system(cmd);
		
	return 0;
}
