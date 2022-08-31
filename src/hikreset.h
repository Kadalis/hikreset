#define NOTVULNERABLE -5
#define NOTENOUGHTMEMORY -4
#define NOTHIKVISIONERR -3
#define EASYPERFORMERR -2
#define CURLINITERR -1

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
int getUsers(char *ip);
int hikvisionCheck(char *ip);
int backdoorCheck(char *ip);
