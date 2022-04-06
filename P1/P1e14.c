#include <stdio.h>
#include <signal.h>
#include <unistd.h>

struct sigaction sa ;
int seconds ;
	
void tic ( int i) {
seconds ++;
alarm (1) ;
}

int main ( void ) {
	int i ,j;
	seconds = 0;
	sa.sa_handler = tic ;
	sigaction ( SIGALRM , &sa , NULL );
	alarm (1) ;
	for (i =0; i <50000; i ++)
		for (j =0; j <100000; j ++);
	printf (" Seconds elapsed = %d\n", seconds );
	return 0;
}
