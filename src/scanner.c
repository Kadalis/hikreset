#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "hikreset.h"

void scan(FILE *fd){
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	while ((read = getline(&line, &len, fd)) != -1) {

		if (*line == '#')
			continue;
		*(line+read-1)='\0'; //we are don't need '\n' symbol, so replacing it with \0
		long checked = backdoorCheck(line);

		if (checked==1)
			printf("%s: Backdoor Confirmed!\n", line);
		else if(checked == -1)
			continue;
		else if (checked == -3){
			printf("Something went wrong during curl init. Proceeding to next target.\n");
			continue;
		}
		else if (checked == -4) continue;
		else fprintf(stderr, "%s: Backdoor not confirmed\n", line);

    }
    free(line);
    fclose(fd);
}