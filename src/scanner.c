#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include "scanner.h"
#include "hikreset.h"

int childCounter = 0;

void SIGCHLDHandler(int sig){
	childCounter--;
	int kek=0;
	wait(&kek);
}

int scan(FILE *fd, int childs){
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	struct sigaction action;
	action.sa_handler = SIGCHLDHandler;
	sigemptyset(&action.sa_mask);
	action.sa_flags=0;
	sigaddset(&action.sa_mask, SIGCHLD);
	sigaction(SIGCHLD, &action, NULL);

	int pid;

	while ((read = getline(&line, &len, fd)) != -1) {
		while (childCounter>=childs)
			SIGCHLDHandler(0);
		
		if (*line == '#') continue;
		//we are don't need '\n' symbol, so replacing it with \0
		if (*(line+read-1)=='\n')
			*(line+read-1)='\0';
		redo:
		if ((pid = fork())==0){
			fclose(fd);
			long checked = backdoorCheck(line);

			if (checked==1){
				printf("%s - Backdoor Confirmed!\n", line);
			}
			else if(checked == -1){
				return 0;
			}
			else if (checked == -3){
				printf("Something went wrong during curl init. Proceeding to next target.\n");
				return 0;
			}
			else if (checked == -4) return 0;
			else fprintf(stderr, "%s - Backdoor not confirmed\n", line);
			free(line);
			return 0;
		}else if (pid>0) childCounter++;
		else goto redo;
    }
    while(childCounter>0)
    	SIGCHLDHandler(0);
    return 1;
    free(line);
    fclose(fd);
}