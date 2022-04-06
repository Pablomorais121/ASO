#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#define B 4096

int main(int argc, char **argv){

	char buffer[B];
	int dforg, dfdst,leidos,escritos;
	
	dforg = open (argv[1], O_RDONLY);
	dfdst = open (argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0600);
	
	do{
		leidos = read(dforg, buffer, B);
		write (dfdst, buffer, leidos);
		printf("%d\n",leidos);
	}while(leidos == B);
	close(dforg);
	close(dfdst);
	return 0;
}
