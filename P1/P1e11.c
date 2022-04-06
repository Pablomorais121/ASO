# include <stdio.h>
# include <signal.h>

struct sigaction sa ;
struct sigaction sa2;

void handler (int sig ) {
printf (" SIGINT received \n");
sigaction(SIGINT, &sa2, NULL);
}



int main ( void ) {
sa . sa_handler = handler ;
sigaction ( SIGINT , &sa , &sa2);
while (1) {}
return 0;
}
