#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#define TAM 11

int main(int argc, char **argv){
	
	char buffer[TAM] = "Hola Mundo\n";
	int file:
	file = open(argv[1], O_WRONLY | O_CREAT, 0600);
	write(file,buffer, TAM);
	close(file);
	return 0;
}
