#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>

enum downloadFile{
	SNAPSHOT,
	CONFIG
};
int backdoorCheck(char *ip);
int download(char *ip, enum downloadFile filetype, FILE *fd);
void getUsers(char *ip);