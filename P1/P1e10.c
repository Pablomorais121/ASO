# include <stdio.h>
# include <signal.h>

struct sigaction sa ;

void handler (int sig ) {
printf (" SIGINT received \n");
sa.sa_handler = SIG_DFL;
sigaction(SIGINT, &sa, NULL);
}

int main (void) {
sa.sa_handler=handler;
sigaction (SIGINT,&sa,NULL);
while (1) {}
return 0;
}
