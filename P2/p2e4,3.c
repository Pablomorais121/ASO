 #include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>
#include <gnu/lib-names.h> 
/*
* Hecho por Pablo Morais Alvarez
*/
       int
       main(void)
       {
      		char cmd[25];
           void *handle;
           double (*cosine)(double);
           char *error;
           
	   printf("\nMemoria antes de emplear el dlopen()\n");
	   sprintf(cmd , "cat /proc/%d/maps", getpid ());
	   system(cmd);
	   
           handle = dlopen(LIBM_SO, RTLD_LAZY);
           if (!handle) {
               fprintf(stderr, "%s\n", dlerror());
               exit(EXIT_FAILURE);
           }

           dlerror();    /* Clear any existing error */

           cosine = (double (*)(double)) dlsym(handle, "cos");


           error = dlerror();
           if (error != NULL) {
               fprintf(stderr, "%s\n", error);
               exit(EXIT_FAILURE);
           }

	   printf("\nMemoria durante el empleo de la libreria\n");
	   sprintf(cmd , "cat /proc/%d/maps", getpid ());
	   system(cmd);
           
           dlclose(handle);
          
           printf("\nMemoria despues de emplear el dlclose()\n");
	   sprintf(cmd , "cat /proc/%d/maps", getpid ());
	   system(cmd);
	   
           exit(EXIT_SUCCESS);
       }
