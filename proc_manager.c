#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_LENGTH 30
#define MAX_LINE 100

int main(){
	char user_input[MAX_LENGTH];
	int index = 0;
	while (fgets(user_input, 30, stdin) != NULL) {
		user_input[strlen(user_input)-1] = 0;//Remove \n character
		index ++;
    	int childID = fork();
    	if (childID == 0){
    		//Start Child process
    		char fileO[MAX_LENGTH] = {};
    		char fileE[MAX_LENGTH] = {};
    		sprintf(fileO, "%d%s", getpid(),".out");
    		sprintf(fileE, "%d%s", getpid(),".err");
   			int fd, fds;
   			fd = open (fileO,O_RDWR|O_CREAT|O_APPEND,0777);
    		dup2(fd, 1);
    		fds = open (fileE,O_RDWR|O_CREAT|O_APPEND,0777);
    		dup2(fds, 2);

    		//populate argument_list[]
    		//used code from https://stackoverflow.com/questions/15472299/split-string-into-tokens-and-save-them-in-an-array
    		char *argument_list[MAX_LENGTH];	
		    int i = 0;
		    char *p = strtok(user_input," ");

		    while (p != NULL)
		    {
		        argument_list[i++] = p;
		        p = strtok (NULL, " ");
		    }
		    argument_list[i] = NULL;


    		printf("Starting command %d: child %d pid of parent %d\n",index,getpid(),getppid());
    		fflush(stdout);
    		fflush(NULL);
			int result = execvp(argument_list[0], argument_list);

			if (result != 0){ 
				exit(2);
			}

    		close(fd);
    		close(fds);
    		exit(0);
    		}//End Child process
	}
	//printf("pressed control + d\n");
	int pid, status;
	//PARENT PROCESS
    while ( (pid = wait(&status)) > 0) {
    	char fileO[MAX_LENGTH] = {};
    	char fileE[MAX_LENGTH] = {};
    	sprintf(fileO, "%d%s", pid,".out");
    	sprintf(fileE, "%d%s", pid,".err");
    	int fd, fds;
    	fd = open (fileO,O_RDWR|O_APPEND,0777);
    	dup2(fd, 1);
    	fds = open (fileE,O_RDWR|O_APPEND,0777);
    	dup2(fds, 2);

    	printf("Finished child %d pid of parent %d\n",pid,getpid());

    	if (WIFEXITED(status)) {
            fprintf(stderr, "Exited with exitcode = %d\n", WEXITSTATUS(status));
     	} else if (WIFSIGNALED(status)) {
            fprintf(stderr, "Killed with signal %d\n", WTERMSIG(status));  
     	}

    	close(fd);
    	close(fds);
    }
	return 0;
}
