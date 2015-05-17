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

#define PAPIER 0
#define ZAPALKI 1
#define TYTON 2

#define verbose

#define CzasPalenia 3
#define CzasDonoszenia 0
#define czasLokalu 15

#if __cplusplus > 199711L
#define LepszyRandom
#endif

#ifdef LepszyRandom
#include <random>
#endif

class Palarnia {
	private:
		pthread_mutex_t mutex;
		pthread_cond_t cond;
		bool pracujemy;
		bool papier;
		bool zapalki;
		bool tyton;   
	public:
		Palarnia() {
			pthread_mutex_init(&mutex, NULL);
			pthread_cond_init(&cond, NULL);
			pracujemy = true;
			papier = false;
			zapalki = false;
			tyton = false;
		}

		~Palarnia() {
			pthread_cond_destroy(&cond);
			pthread_mutex_destroy(&mutex);
		}

		void przyniesWszystkoProcz(unsigned int brakujacyElement) {
			pthread_mutex_lock(&mutex);
			while (pracujemy && (papier || zapalki || tyton)) {
				#ifdef verbose
				printf("LOKAJ: Panowie jeszcze maja, to ja poczekam...\n");
				#endif
				pthread_cond_wait(&cond, &mutex);
			}
			if (!pracujemy) {
				pthread_mutex_unlock(&mutex);
				return;
			}
			if (brakujacyElement == PAPIER) {
				#ifdef verbose
				printf("LOKAJ: Przynioslem zapalki i tyton!\n");
				#endif
				//papier = true;
				zapalki = true;
				tyton = true;
			} else if (brakujacyElement == ZAPALKI) {
				#ifdef verbose
				printf("LOKAJ: Przynioslem papier i tyton!\n");
				#endif
				papier = true;
				//zapalki = true;
				tyton = true;
			} else if (brakujacyElement == TYTON) {
				#ifdef verbose
				printf("LOKAJ: Przynioslem papier i zapalki!\n");
				#endif
				papier = true;
				zapalki = true;
				//tyton = true;
			}
			pthread_cond_broadcast(&cond);
			pthread_mutex_unlock(&mutex);
		}

		void zapalSobie(unsigned int coMamy) {
			pthread_mutex_lock(&mutex);
			bool* pierwszyBrakujacyElement;
			bool* drugiBrakujacyElement;

			if (coMamy == PAPIER) {
				pierwszyBrakujacyElement = &zapalki;
				drugiBrakujacyElement = &tyton;
			} else if (coMamy == ZAPALKI) {
				pierwszyBrakujacyElement = &papier;
				drugiBrakujacyElement = &tyton;
			} else if (coMamy == TYTON) {
				pierwszyBrakujacyElement = &papier;
				drugiBrakujacyElement = &zapalki;
			}

			while (pracujemy && (!*pierwszyBrakujacyElement || !*drugiBrakujacyElement)) {
				#ifdef verbose
				printf("PALACZ %u: Czekam...\n", coMamy);
				#endif
				pthread_cond_wait(&cond, &mutex);
			}
			if (!pracujemy) {
				pthread_mutex_unlock(&mutex);
				return;
			}

			#ifdef verbose
			printf("PALACZ %u: Moja kolej! :)\n", coMamy);
			#endif
			papier = false;
			zapalki = false;
			tyton = false;
			pthread_cond_broadcast(&cond);
			pthread_mutex_unlock(&mutex);
		}

		void aktualnyStan() {
			printf("----\n");
			if (papier) {
				printf("PAPIER: TAK\n");
			} else {
				printf("PAPIER: NIE\n");
			}
			if (zapalki) {
				printf("ZAPALKI: TAK\n");
			} else {
				printf("ZAPALKI: NIE\n");
			}
			if (tyton) {
				printf("TYTON: TAK\n");
			} else {
				printf("TYTON: NIE\n");
			}
			printf("----\n");
		}

		void zamykamy() {
			pthread_mutex_lock(&mutex);
			pracujemy = false;
			pthread_cond_broadcast(&cond);
			pthread_mutex_unlock(&mutex);
		}

		bool czyPracujemy() {
			pthread_mutex_lock(&mutex);
			bool ret = pracujemy;
			pthread_mutex_unlock(&mutex);
			return ret;
			//return pracujemy;
		}
};

Palarnia palarnia;

void* palaczBezPapieru(void *) {
	while (palarnia.czyPracujemy()) {
		palarnia.zapalSobie(PAPIER);
		#if CzasPalenia > 0
		sleep(CzasPalenia);
		#endif
		#ifdef verbose
		printf("PALACZ %d: Skonczylem palic!\n", PAPIER);
		#endif
	}
	return NULL;
}

void* palaczBezZapalek(void *) {
	while (palarnia.czyPracujemy()) {
		palarnia.zapalSobie(ZAPALKI);
		#if CzasPalenia > 0
		sleep(CzasPalenia);
		#endif
		#ifdef verbose
		printf("PALACZ %d: Skonczylem palic!\n", ZAPALKI);
		#endif
	}
	return NULL;
}

void* palaczBezTytoniu(void *) {
	while (palarnia.czyPracujemy()) {
		palarnia.zapalSobie(TYTON);
		#if CzasPalenia > 0
		sleep(CzasPalenia);
		#endif
		#ifdef verbose
		printf("PALACZ %d: Skonczylem palic!\n", TYTON);
		#endif
	}
	return NULL;
}

void* lokaj(void *) {
	unsigned int brakujacyElement;
	#ifdef LepszyRandom
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<double> dist(1, 4);
	#endif
	while (palarnia.czyPracujemy()) {
		#ifdef LepszyRandom
		brakujacyElement = dist(mt);
		#else
		brakujacyElement = rand() % 3;
		#endif
		palarnia.przyniesWszystkoProcz(brakujacyElement);
		#if CzasDonoszenia > 0
		sleep(CzasDonoszenia);
		#endif
	}
	return NULL;
}

void* straznik(void *) {
	while (palarnia.czyPracujemy()) {
		palarnia.aktualnyStan();
		sleep(1);
	}
	return NULL;
}

int main() {
	std::vector <pthread_t*> watki;
	pthread_t* idWatku;

	// palaczBezPapieru
	idWatku = new pthread_t;
	watki.push_back(idWatku);
	pthread_create(idWatku, NULL, palaczBezPapieru, (void*) NULL);

	// palaczBezZapalek
	idWatku = new pthread_t;
	watki.push_back(idWatku);
	pthread_create(idWatku, NULL, palaczBezZapalek, (void*) NULL);

	// palaczBezTytoniu
	idWatku = new pthread_t;
	watki.push_back(idWatku);
	pthread_create(idWatku, NULL, palaczBezTytoniu, (void*) NULL);

	// lokaj
	idWatku = new pthread_t;
	watki.push_back(idWatku);
	pthread_create(idWatku, NULL, lokaj, (void*) NULL);

	// straznik
	/*
	idWatku = new pthread_t;
	watki.push_back(idWatku);
	pthread_create(idWatku, NULL, straznik, (void*) NULL);
	*/

	sleep(czasLokalu);
	palarnia.zamykamy();
	for (unsigned int i = 0; i < watki.size(); ++i) {
		pthread_join(*watki[i], NULL);
		delete watki[i];
	}

	printf("Wszystkie watki zakonczyly sie powodzeniem :)\n");
	return 0;
}



