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

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h> 
#include <unistd.h>
#include <vector>

#define czasDzialania 6 // Ile sekund program ma dzialac

#define iluProducentow 10 // Ile watkow produkuje
#define iluKonsumentow 10 // Ile watkow konsumuje

#define ileWstawiamy 0 // Ile produktow wstawia pojedynczy watek, 0 -> random
#define ilePobieramy 0 // Ile produktow pobiera pojedynczy watek, 0 -> random
#define ileMax 1000 // Maksymalna pojemnosc "magazynu", musi byc wieksza lub rowna ilePobieramy, w przeciwnym wypadku bedzie deadlock :)
#define ileSpimy 0 // Dlugosc drzemki pojedynczego watku po skonczonej robocie, 0 -> brak

unsigned int pracujemy = 1;

class Bufor {
	private:
		pthread_mutex_t mutex;
		pthread_cond_t cond;
		unsigned int ileDostepnych;
    
	public:
		Bufor() {
			pthread_mutex_init(&mutex, NULL);
			pthread_cond_init(&cond, NULL);
		}

		~Bufor() {
			pthread_cond_destroy(&cond);
			pthread_mutex_destroy(&mutex);
		}

		void WstawElement(unsigned int ile) {
			pthread_mutex_lock(&mutex);
			while (pracujemy && ileDostepnych >= ileMax) {
				printf("PRODUCENT: Mamy: %u, Osiagnelismy max: %u, czekam...\n", ileDostepnych, ileMax);
				pthread_cond_wait(&cond, &mutex);
			}
			if (pracujemy) {
				ileDostepnych += ile;
				printf("PRODUCENT: Wstawiono: %u, Dostepnych: %u\n", ile, ileDostepnych);
			} else {
				printf("PRODUCENT: Ale juz sie nie doczekam dzisiaj :(\n");
			}
			pthread_cond_broadcast(&cond);
			pthread_mutex_unlock(&mutex);
		}

		void PobierzElement(unsigned int ile) {
			pthread_mutex_lock(&mutex);
			while (pracujemy && ileDostepnych < ile) {
				printf("KONSUMENT: Chcemy: %u, Dostepnych: %u, czekam...\n", ile, ileDostepnych);
				pthread_cond_wait(&cond, &mutex);
			}
			if (pracujemy) {
				ileDostepnych -= ile;
				printf("KONSUMENT: Pobrano %u, Dostepnych: %u\n", ile, ileDostepnych);
			} else {
				printf("KONSUMENT: Ale juz sie nie doczekam dzisiaj :(\n");
			}
			pthread_cond_broadcast(&cond);
			pthread_mutex_unlock(&mutex);
		}

		void KoniecRoboty() {
			pthread_mutex_lock(&mutex);
			printf("INFO: KONIEC ROBOTY\n");
			pracujemy = 0;
			pthread_cond_broadcast(&cond);
			pthread_mutex_unlock(&mutex);
		}
};

Bufor buf;

void* producent(void *) {
	unsigned int ile = ileWstawiamy;
	while (pracujemy) {
#if ileWstawiamy < 1
		ile = rand() % 100;
#endif
		buf.WstawElement(ile);
#if ileSpimy > 0
		sleep(ileSpimy);
#endif
	}
	return NULL;
}

void* konsument(void *) {
	unsigned int ile = ilePobieramy;
	while (pracujemy) {
#if ilePobieramy < 1
		ile = rand() % 100;
#endif
		buf.PobierzElement(ile);
#if ileSpimy > 0
		sleep(ileSpimy);
#endif
	}
	return NULL;
}

int main() {
	std::vector <pthread_t*> watki;
	pthread_t* idWatku;

	for (unsigned int i = 0; i < iluKonsumentow; ++i) {
		idWatku = new pthread_t;
		watki.push_back(idWatku);
		pthread_create(idWatku, NULL, konsument, (void*) NULL);
	}
	for (unsigned int i = 0; i < iluProducentow; ++i) {
		idWatku = new pthread_t;
		watki.push_back(idWatku);
		pthread_create(idWatku, NULL, producent, (void*) NULL);
	}

	sleep(czasDzialania);
	buf.KoniecRoboty();
	for (unsigned int i = 0; i < watki.size(); ++i) {
		pthread_join(*watki[i], NULL);
		delete watki[i];
	}

	printf("Wszystkie watki zakonczyly sie powodzeniem :)\n");
	return 0;
}



