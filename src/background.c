// background.c
// author: David Jones

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void usage() {
	printf("usage:  background command args ...");
	exit(1);
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		usage();
	}
	pid_t pid = fork();
	if (pid < 0) {
		perror("background - fork");
		exit(1);
	}
	if (pid) {
		exit(0);
	}
	// child process
	setpgid(0,0);
	execvp(argv[1], &argv[1]);
	perror("background - execvp");
	exit(1);	
}
