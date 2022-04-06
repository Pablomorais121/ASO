#include <stdio.h>
#include <unistd.h>

int main ( void ) {
	int pid = fork () ;
	if ( pid ==0){
		sleep(1);
		printf ("[H] ppid = %5d, pid = %5d\n", getppid () , getpid () );
	}else{
		printf ("[P] ppid = %5d, pid = %5d, H = %5d\n", getppid () , getpid () , pid );
	}
	return 0;
}

