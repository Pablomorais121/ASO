#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

int gb;
bool gbbt = true;
int gb2 = 10;
bool gbbf;

void main(){
	int lc = 20;
	int lc2;
	bool lcbt = true;
	bool lcbf;
	
	char cmd[25];
	sprintf(cmd, "cat /proc/%d/maps",getpid());
	system(cmd);
	
	
	
	 printf("--\n");
    printf("Variable/función   Dirección              Tamaño\n");
    printf("            main   %p\n", main);
    printf("           errno   %p   %12ld\n", &errno, sizeof(errno));
    printf("       	      gb   %p   %12ld\n", &gb, sizeof(gb));
    printf("            gbbt   %p   %12ld\n", &gbbt, sizeof(gbbt));
    printf("             gb2   %p   %12ld\n", &gb2, sizeof(gb2));
    printf("            gbbf   %p   %12ld\n", &gbbf, sizeof(gbbf));
    printf("              lc   %p   %12ld\n", &lc, sizeof(lc));
    printf("             lc2   %p   %12ld\n", &lc2, sizeof(lc2));
    printf("            lcbt   %p   %12ld\n", &lcbt, sizeof(lcbt));
    printf("            lcbf   %p   %12ld\n", &lcbf, sizeof(lcbf));


	exit;
}
