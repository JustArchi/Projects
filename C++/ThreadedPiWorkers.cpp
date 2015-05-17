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
#include <sys/time.h>

#if __cplusplus > 199711L
#include <thread>
#endif

typedef struct _Arg {
	pthread_t* thread;
	double step;
	unsigned int startI;
	unsigned int endI;
	double wynik;
} Argument;

void* PiWorker(void* _arg) {
	Argument* arg = (Argument*)_arg;
	double x, sum = 0.0;
	for (int i = arg->startI; i < arg->endI; i++) {
		x = (i + 0.5) * arg->step;
		sum = sum + 4.0 / (1.0 + x * x);
	}
	arg->wynik = sum;
	return NULL;
}

int main(int argc, char* argv[]) {

#if __cplusplus > 199711L
	unsigned int threadsCount = std::thread::hardware_concurrency();
#else
	unsigned int threadsCount = 1;
#endif

	if (argc > 1) {
		threadsCount = atoi(argv[1]);
	}

	if (threadsCount < 1) {
		printf("Invalid number of threads!\n");
		return 1;
	}

	struct timeval startTime, endTime;

	printf("Number of threads: %u\n", threadsCount);
	unsigned long long num_steps = 1000000000;

	double step = 1.0 / (double) num_steps;
	unsigned long long growI = num_steps / threadsCount;
	unsigned long long startI = 0;
	unsigned long long endI = growI;

	double sum = 0.0;
	std::vector <Argument*> v;

	gettimeofday(&startTime, NULL);
	for (unsigned int i = 0; i < threadsCount; i++) {
		Argument* arg = new Argument;
		v.push_back(arg);
		arg->thread = new pthread_t;
		arg->step = step;
		arg->startI = startI;
		arg->endI = endI;
		pthread_create(arg->thread, NULL, PiWorker, arg);
		startI += growI;
		endI += growI;
	}

	for (unsigned int i = 0; i < threadsCount; i++) {
		pthread_join(*v[i]->thread, NULL);
		sum += v[i]->wynik;
	}

	sum *= step;
	gettimeofday(&endTime, NULL);

	printf("Result: %f\n", sum);

	long seconds  = endTime.tv_sec  - startTime.tv_sec;
	long useconds = endTime.tv_usec - startTime.tv_usec;
	double duration = seconds + useconds/1000000.0;

	printf("It took %f seconds\n", duration);
	return 0;
}

