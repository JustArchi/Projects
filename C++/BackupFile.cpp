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

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <pthread.h>
#include <omp.h>

#include <vector>

#include <iostream>
#include <fcntl.h>   // open
#include <unistd.h>  // read, write, close
#include <cstdio>    // BUFSIZ

//#define USESYSTEM

char* sourceFile;
char* outFile;

char* copycat(char* foo, char* bar) {
	size_t size = strlen(foo) + strlen(bar) + 1;
	char* result = new char[size];

	strcpy(result, foo);
	strcat(result, bar);

	return result;
}

void fnExit(void) {
#ifdef USESYSTEM
	string command;
	command += "rm ";
	command += sourceFile;
	system(command.c_str());
#else
	remove(sourceFile);
#endif
}

int main(int argc, char* argv[]) {
	if (argc > 1) {
		sourceFile = argv[1];
	} else {
		sourceFile = argv[0];
	}
	outFile = copycat(sourceFile, (char*) ".bak");
	atexit(fnExit);
#ifdef USESYSTEM
	string command;
	command += "cp ";
	command += sourceFile;
	command += " ";
	command += outFile;
	system(command.c_str());
#else
	char buf[BUFSIZ];
	size_t size;

	int source = open(sourceFile, O_RDONLY, 0);
	int dest = open(outFile, O_WRONLY | O_CREAT /*| O_TRUNC/**/, 0644);

	while ((size = read(source, buf, BUFSIZ)) > 0) {
		write(dest, buf, size);
	}

	close(source);
	close(dest);
#endif
	return 0;
}

