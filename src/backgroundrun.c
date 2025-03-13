/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2016 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/
 /*
 * @author David Jones
 */
// background.c

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
