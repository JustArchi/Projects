/*
      _           _      _             _     _
     | |_   _ ___| |_   / \   _ __ ___| |__ (_)
  _  | | | | / __| __| / _ \ | '__/ __| '_ \| |
 | |_| | |_| \__ \ |_ / ___ \| | | (__| | | | |
  \___/ \__,_|___/\__/_/   \_\_|  \___|_| |_|_|

 Copyright 2014 Łukasz "JustArchi" Domeradzki
 Contact: JustArchi@JustArchi.net

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/

#include <cstdlib>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>

static const unsigned int nProc = 4; // Equal or greater than 2. Defines number of processes in the chain
static const unsigned int nBalls = 3; // Equal or greater than 1. Defines number of balls in the chain
static const unsigned int nPasses = 15; // Equal or greater than 0. Defines number of passes until ball is erased. Non-final, as master deletes ball when it has >= nPasses
static const unsigned int nSleep = 1; // Sleep between forwarding a ball

//#define VERBOSE
#define SEGSIZE 100

bool master = false;
unsigned int howManyPassed = 0;
unsigned int nBallsLive = 0;

int reportStatistics() {
	key_t key;
	int shmid, cntr;
	char *segptr;

	key = ftok(".", '$');
#ifdef VERBOSE
	printf("DEBUG: Key: %d\n", key);
#endif

	if ((shmid = shmget(key, SEGSIZE, IPC_CREAT | 0666)) < 0) {
		fprintf(stderr, "ERROR: Failed to create shared memory segment\n");
		return EXIT_FAILURE;
	}

	if ((segptr = (char *) shmat(shmid, NULL, 0)) == (char *) -1) {
		fprintf(stderr, "ERROR: Failed to attach shared memory segment\n");
		return EXIT_FAILURE;
	}

	memset(segptr, 0, SEGSIZE);
	sprintf(segptr, "Statistics:\Passed: %u balls\n", howManyPassed);
#ifdef VERBOSE
	printf("Saved message:\n%s", segptr);
#endif
}

void parseSignal(int signum) {
	fprintf(stderr, "Caught signal: %d\n", signum);
	reportStatistics();
}

int startPingPong(int* OUT) {
	unsigned int c = 1;
#ifdef VERBOSE
	printf("START\n");
#endif
	for (unsigned int i = 0; i < nBalls; ++i) {
		write(*OUT, &c, sizeof(c));
		++nBallsLive;
	}
	return 0;
}

int pingPong(int* IN, int* OUT) {
	unsigned int c;
	while (1) {
		read(*IN, &c, sizeof(c));

		if (c >= nPasses && master) { // master removing ball
#ifdef VERBOSE
			printf("Removing ball!\n");
#endif
			--nBallsLive;
			if (nBallsLive <= 0) {
#ifdef VERBOSE
				printf("This was the last ball!\n");
#endif
				c = 0;
				write(*OUT, &c, sizeof(c)); // broadcast the finish
			}
		} else { // forward ball
			if (c > 0) { // ball has arrived
#ifdef VERBOSE
				printf("PING: %u\n", c);
#endif
				++howManyPassed;
				++c;
				sleep(nSleep);
				write(*OUT, &c, sizeof(c));
#ifdef VERBOSE
				printf("PONG: %u\n", c);
#endif
			} else { // this is finish broadcast
				write(*OUT, &c, sizeof(c)); // broadcast further
				return 0; // exit
			}
		}
	}
	return 0;
}

int main() {

	signal(SIGUSR1, parseSignal);

	unsigned int n = 0;
	int fd[nProc][2];
	int* IN;
	int* OUT;

	for (unsigned int i = 0; i < nProc; ++i) {
		if (pipe(fd[i]) < 0) {
			fprintf(stderr, "Pipe error\n");
			return EXIT_FAILURE;
		}
	}

	for (unsigned int i = 0; i < nProc - 1; ++i) {
		pid_t pid = fork();
		if (pid < 0) {
			fprintf(stderr, "Fork error\n");
			return EXIT_FAILURE;
		} else if (pid == 0) { //child process
			n++;
		} else { // parent process
			n = i;
			break;
		}
	}

	if (n == 0) {
		IN = &fd[nProc - 1][0];
	} else {
		IN = &fd[n-1][0];
	}
	OUT = &fd[n][1];
	for (unsigned int i = 0; i < nProc; ++i) {
		if (fd[i][0] != *IN) {
			if (close(fd[i][0]) < 0) {
				fprintf(stderr, "Pipe close error\n");
				return EXIT_FAILURE;
			}
		}
		if (fd[i][1] != *OUT) {
			if (close(fd[i][1]) < 0) {
				fprintf(stderr, "Pipe close error\n");
				return EXIT_FAILURE;
			}
		}
	}

	if (n == 0) {
		master = true;
		startPingPong(OUT);
	}
	pingPong(IN, OUT);
	return 0;
}