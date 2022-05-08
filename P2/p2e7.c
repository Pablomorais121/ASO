#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

/*
* Hecho por Pablo Morais Alvarez
*/

char cmd[25];

void main(){

	printf("Memoria antes de emplear el exec()\n");
	sprintf(cmd, "cat /proc/%d/maps",getpid());
	system(cmd);
	printf("\n\n");
	
	int pid = fork();
	
	if(pid == 0){
		printf("Memoria del exec\n");
		int ret = execl("/bin/cat","cat","/proc/self/maps", (char *) 0);
	}else{
		sleep(1);
		printf("\n\n");
		printf("Memoria del padre\n");
		sprintf(cmd, "cat /proc/%d/maps",getpid());
		system(cmd);
		printf("\n\n");
	}
	
}
