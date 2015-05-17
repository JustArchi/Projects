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

:- dynamic przedmiot/2, otwarte/1, jestem_w/1, ilemap/1.


/* START */
jestem_w(parking).
ilemap(0).

/* PARKING */
droga(parking, zachod, budynekA).
droga(parking, wschod, budynekC) :- otwarte(budynekCD).
droga(parking, wschod, budynekC) :- przedmiot(kluczC, ekwipunek_gracza), retract(przedmiot(kluczC, ekwipunek_gracza)), assert(otwarte(budynekCD)).
droga(parking, wschod, budynekC) :- write('Drzwi do budynku C sa zamkniete, podejrzane...'), nl, fail.
opisz(parking) :-
        write('Trafiles na parking PJWSTK'), nl,
		write('Wokol ani zywej duszy, czyzby wszyscy studenci poszli juz na zajecia? Niee, to niemozliwe...'), nl,
        nl,
        write('zachod - idz do budynku A'), nl,
        write('wschod - idz do budynku C'), nl,
		nl.

/* BUDYNEK A */
przedmiot(kluczC110, budynekA).
droga(budynekA, zachod, budynekAschody) :- otwarte(budynekAschodyD).
droga(budynekA, zachod, budynekAschody) :- przedmiot(kluczAschody, ekwipunek_gracza), write('Pomimo posiadania dobrego klucza wyglada na to, ze drzwi mozna otworzyc tylko od wewnatrz...'), nl, fail.
droga(budynekA, zachod, budynekAschody) :- write('Drzwi sa zamkniete z naklejona karteczka "klucz w dziekanacie"...'), nl, fail.
droga(budynekA, wschod, budynekAwyklad).
droga(budynekA, naprzod, budynekA2).
droga(budynekA, wstecz, parking).
opisz(budynekA) :-
        write('Trafiles do budynku A'), nl,
        nl,
        write('zachod - idz w kierunku schodow'), nl,
        write('wschod - idz do sali wykladowej A1'), nl,
        write('naprzod - idz wglab'), nl,
        write('wstecz - wroc na parking'), nl,
		nl.

/* BUDYNEK C */
przedmiot(mapa, budynekC).
droga(budynekC, wstecz, parking).
opisz(budynekC) :-
        write('Trafiles do budynku C'), nl,
        nl,
        write('Historia toczylaby sie dalej, ale niestety jest juz grubo po polnocy wiec nie ma na to czasy'), nl,
        write('wstecz - wroc na parking'), nl,
		nl.

/* BUDYNEK Awyklad */
przedmiot(mapa, budynekAwyklad).
przedmiot(kluczC, budynekAwyklad).
mainterakcje(mapa).
droga(budynekAwyklad, wstecz, budynekA).
opisz(budynekAwyklad) :-
		write('Wchodzisz na sale wykladowa, wyglada na to ze wyklad juz sie skonczyl'), nl,
		nl,
		write('wstecz - wroc na poczatek budynku A'), nl,
		nl.

/* SCHODY */


/* BUDYNEK A2 */
droga(budynekA2, zachod, budynekAwinda).
droga(budynekA2, naprzod, budynekAdziekanat).
droga(budynekA2, wstecz, budynekA).
opisz(budynekA2) :-
		write('Idziesz wglab budynku A'), nl,
		nl,
		write('zachod - idz do windy'), nl,
		write('naprzod - idz do dziekanatu'), nl,
		write('wstecz - wroc na poczatek budynku A'), nl,
		nl.

/* BUDYNEK Awinda */
droga(budynekAwinda, wstecz, budynekA2).
przedmiot(mapa, budynekAwinda).
opisz(budynekAwinda) :-
		write('Trafiasz do windy, wyglada na to ze dziala'), nl,
		write('Dostepne przyciski: -1 [0] 1 2 3'), nl,
		nl,
		write('wcisnij(X) - wcisnij przycisk oznaczajacy pietro'), nl,
		write('wstecz - wroc do budynku A'), nl,
		nl.

/* BUDYNEK Adziekanat */
przedmiot(kluczAschody, budynekAdziekanat).
droga(budynekAdziekanat, wstecz, budynekA2).
opisz(budynekAdziekanat) :-
		write('Wchodzisz do dziekanatu, wszyscy wygladaja jakby chcieli Cie zabic, lepiej szybko stad znikaj...'), nl,
		nl,
		write('wstecz - wroc do budynku A'), nl,
		nl.

/* BUDYNEK Aminus */
droga(budynekAminus, naprzod, koniecGry).
opisz(budynekAminus) :-
		write('Trafiasz na pietro -1, juz tylko chwila dzieli Cie od dotarcia do celu...'), nl,
		write('Winda stala sie calkiem niesprawna, nie ma opcji zeby nia wrocic na gore'), nl,
		nl,
		write('naprzod - idz wglab'), nl,
		nl.

/* BUDYNEK Aminus */
opisz(koniecGry) :-
		write('Gratulacje! Trafiles do sali i tym samym zakonczyles gre!'), nl,
		koniec.

wiecejmap :-
	retract(przedmiot(mapa, ekwipunek_gracza)),
	ilemap(X),
	X1 is X+1,
	retract(ilemap(X)),
	assert(ilemap(X1)).

interakcja(mapa) :-
	ilemap(0),
	write('Uzywasz pierwszego z trzech elementow ukladanki...'), nl,
	write('Otrzymujesz nowa komende, .mapa - aby przeczytac jak wyglada aktualna mapa'), nl,
	wiecejmap,
	mapa.

interakcja(mapa) :-
	write('Znajdujesz kolejny element ukladanki!'), nl,
	wiecejmap,
	mapa.

mapa :-
	ilemap(1),
	write('Posiadasz aktualnie 1 skrawek mapy. Odczytac z niego mozna: K.d d. w..d. t. .1.'), nl.

mapa :-
	ilemap(2),
	write('Posiadasz aktualnie 2 skrawki mapy. Odczytac z nich mozna: Kod d. w.ndy t. 21.'), nl.

mapa :-
	ilemap(3),
	write('Posiadasz kompletna mape. Odczytac z niej mozna: Kod do windy to 210'), nl.

/* KODY DO WINDY */
kod(-1).
kod(0).
kod(1).
kod(2).
kod(3).
kod(210).

/* AKCJA - WCISNIJ */
wcisnij(X) :-
	jestem_w(budynekAwinda),
	kod(210),
	write('Winda nagle zamknela drzwi i ruszyla w dol...'), nl, 
	retract(jestem_w(budynekAwinda)),
	assert(jestem_w(budynekAminus)),
	rozejrzyjsie.

wcisnij(X) :-
	jestem_w(budynekAwinda),
	kod(X),
	write('Pomimo wcisniecia dobrego przycisku, winda nie reaguje'), nl.

wcisnij(X) :-
	jestem_w(budynekAwinda),
	write('Nie ma takiego przycisku'), nl.

wcisnij(_) :-
	write('Ta komenda jest aktualnie niedostepna'), nl.

/* AKCJA - PODNIES */
podnies(X) :-
        przedmiot(X, ekwipunek_gracza),
        write('Juz posiadasz ten przedmiot!'),
        nl.

podnies(X) :-
        jestem_w(Miejsce),
        przedmiot(X, Miejsce),
        retract(przedmiot(X, Miejsce)),
        assert(przedmiot(X, ekwipunek_gracza)),
        write('Podniosles przedmiot: '), write(X), write(' z miejsca: '), write(Miejsce),
        nl.

podnies(_) :-
        write('Nie widze tutaj niczego takiego'),
        nl.

/* AKCJA - UPUSC */
upusc(X) :-
        przedmiot(X, ekwipunek_gracza),
        jestem_w(Miejsce),
        retract(przedmiot(X, ekwipunek_gracza)),
        assert(przedmiot(X, Miejsce)),
        write('Upusciles przedmiot: '), write(X), write(' w miejscu: '), write(Miejsce),
        nl.

upusc(_) :-
        write('Nie trzymasz nic takiego'),
        nl.

/* AKCJA - UZYJ */
uzyj(X) :-
        przedmiot(X, ekwipunek_gracza),
        mainterakcje(X),
        interakcja(X),
        nl.

uzyj(X) :-
        przedmiot(X, ekwipunek_gracza),
        write('Nie mozna uzyc tego przedmiotu!'),
        nl.

uzyj(_) :-
        write('Nie posiadasz takiego przedmiotu!'),
        nl.


/* AKCJA - IDZ */
naprzod :- idz(naprzod).
wstecz :- idz(wstecz).
wschod :- idz(wschod).
zachod :- idz(zachod).

idz(Kierunek) :-
        jestem_w(Tu),
        droga(Tu, Kierunek, Tam),
        retract(jestem_w(Tu)),
        assert(jestem_w(Tam)),
        rozejrzyjsie.

/*idz(_) :- write('Nie mozesz pojsc w tym kierunku!').*/


/* AKCJA - ROZEJRZYJ SIE */
rozejrzyjsie :-
        jestem_w(Miejsce),
        opisz(Miejsce),
        zauwaz_obiekty(Miejsce).

/* AKCJA - ZAUWAZ_OBIEKTY */
zauwaz_obiekty(Miejsce) :-
        przedmiot(X, Miejsce),
        write('Lezy tutaj '), write(X), write('!'), nl,
        fail.

zauwaz_obiekty(_).

/* AKCJA - KONIEC */
koniec :-
        nl,
        write('Gra sie zakonczyla, wpisz .halt aby zakonczyc'),
        nl.

/* AKCJA - POMOC */
pomoc :-
	nl,
	write('Witaj smiertelniku, twoim zadaniem jest dotrzec do sali S03 znajdujacej sie na pietrze -1'), nl,
	write('Brzmi prosto? Zaraz sie o tym przekonamy!'), nl,
	nl,
	write('Dostepne komendy:'), nl,
	write('start.                                   - Startuje gre'), nl,
	write('naprzod. wstecz. wschod. zachod. idz(X). - Idz w tym kierunku'), nl,
	write('podnies(obiekt).                         - Podnosi dany obiekt'), nl,
	write('uzyj(obiekt).                            - Wchodzi w interakcje z danym obiektem z twojego ekwipunku'), nl,
	write('rozejrzyjsie.                            - Wyswietl jeszcze raz informacje o aktualnym miejscu'), nl,
	write('pomoc.                                   - Wyswietla ten tekst jeszcze raz'), nl,
	write('koniec.                                  - Konczy gre'), nl,
	pomocmapa,
	nl.

pomocmapa :-
	ilemap(X), X > 0,
	write('mapa.                                    - Odczytuje mape'), nl.

pomocmapa :-
	true.

/* AKCJA - START */
start :-
        pomoc,
        rozejrzyjsie.