#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main ( void ) {
	int status;
	int pid = fork () ;
	if ( pid ==0){
		sleep(2);
		printf ("[H] ppid = %5d, pid = %5d\n", getppid () , getpid () );
		exit(1);
	}else{
		printf ("[P] ppid = %5d, pid = %5d, H = %5d\n", getppid () , getpid () , pid );
		wait(&status);
		printf("[P] el proceso pid = %d acaba de terminar con estado %d\n",getpid(),status);
	}
	return 0;
}

