#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void main(){
	
	char cmd[25];
	
	printf("Memoria antes de emplear el fork()\n");
	sprintf(cmd, "cat /proc/%d/maps",getpid());
	system(cmd);
	printf("\n\n");
	
	
	int pid = fork();
	
	if(pid == 0){
		printf("Memoria del hijo\n");
		sprintf(cmd, "cat /proc/%d/maps",getpid());
		system(cmd);
		printf("\n\n");
	}else{
	sleep(1);
		printf("Memoria del padre\n");
		sprintf(cmd, "cat /proc/%d/maps",getpid());
		system(cmd);
		printf("\n\n");
	}
}
