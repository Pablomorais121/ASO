#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
pid_t padre;
pid_t pids[5];
for(int i = 0; i < 5;i++){
	padre = fork();
		if(padre == -1){
 		perror("Error en la llamada fork()");
 		}else if( padre == 0){
 			switch(i){
 			
 			case 0:
 			execl("/usr/bin/xload","xload",(char *)0);
 			break;
 			
 			case 1:
 			execl("/usr/bin/xeyes","xeyes",(char *)0);
 			break;	
 			
 			case 2:
 			execl("/usr/bin/xlogo","xlogo",(char *)0);
 			break;
 			
 			case 3:
 			execl("/usr/bin/xcalc","xcalc",(char *)0);
 			break;
 			
 			case 4:
 			execl("/usr/bin/xclock","xclocl -update 1",(char *)0);
 			break;
 			
 			
 			}	
 		}else{
		pids[i] = padre;
		}
}
int status;
padre = wait(&status);
while(padre != -1){
	pid_t newp;
	newp = fork();
	if(newp == -1){
	perror("Error en la llamada fork()");
	}else if(newp ==0){
	
		if(padre == pids[0]){
		execlp("xload","xload",(char *)0);
		}else if(padre == pids[1]){
		execlp("xeyes","xeyes",(char *)0);
		}else if(padre == pids[2]){
		execlp("xlogo","xlogo",(char *)0);
		}else if(padre == pids[3]){
		execlp("xcalc","xcalc",(char *)0);
		}else if(padre == pids[4]){
		execlp("xclock","xclocl -update 1",(char *)0);
		}
	}else{
		if(padre == pids[0]){
		pids[0] = newp;
		}else if(padre == pids[1]){
		pids[1] = newp;
		}else if(padre == pids[2]){
		pids[2] = newp;
		}else if(padre == pids[3]){
		pids[3] = newp;
		}else if(padre == pids[4]){
		pids[4] = newp;
		}			
	
	}
	padre = wait(&status);
}
return 0;
}	
