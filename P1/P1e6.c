#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main ( void ) {
	printf("Soy un calendario en C, mi pid es %d\n",getpid());
	int rec;
	rec = execlp("cal","cal","2022",(char *)0);
	printf("Soy un calendario en C, mi pid es %d\n",getpid());
	return 0;
}

