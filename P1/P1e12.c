# include <stdio.h>
# include <signal.h>

struct sigaction sa;
struct sigaction sa2;
struct sigaction sa3;

void handler (int sig ) {
printf (" SIGINT received 1 \n");
sigaction(SIGINT, &sa2, &sa3);
}

void handler2 (int sig){
printf("SIGINT received 2 \n");
sigaction(SIGINT,&sa3, NULL);
}



int main ( void ) {
sa.sa_handler = handler;
sa2.sa_handler = handler2;
sigaction ( SIGINT , & sa ,NULL);
while (1) {}
return 0;
}
