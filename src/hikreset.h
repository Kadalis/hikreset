#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>

enum downloadFile{
	SNAPSHOT,
	CONFIG
};

struct MemoryStruct {
  char *memory;
  size_t size;
};

int download(char *ip, enum downloadFile filetype, FILE *fd);
void getUsers(char *ip);
int hikvisionCheck(char *ip);
int backdoorCheck(char *ip);
