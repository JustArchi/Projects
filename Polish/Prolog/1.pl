:- dynamic marriage/2.
:- dynamic age/2.

man(adam).
man(wojtek).
man(tadek).
man(kuba).

woman(ewa).
woman(ola).
woman(jadzia).

marriage(adam, ewa).
marriage(kuba, jadzia).

age(wojtek, 17).

divorce(X,Y) :- marriage(X,Y), retract(marriage(X,Y)), write(X), write(' nie jest juz mezem '), write(Y).
divorce(X,Y) :- not(marriage(X,Y)), write('nie sa malzenstwem').

pary :- write('obecne malzenstwa'), nl, marriage(X,Y), write(X), write(' '), write(Y), nl, fail.
kandydaci :- write('obecni kandydaci'), nl, man(X), not(marriage(X,Y)), write(X), nl, fail.
kandydatki :- write('obecne kandydatki'), nl, woman(Y), not(marriage(X,Y)), write(Y), nl, fail.

getolder(X) :- age(X,Y), Y1 is Y+1, retract(age(X,Y)), assert(age(X,Y1)), write('rok pozniej...').

wedding(X,Y) :- not(marriage(X,Z)), not(marriage(C,Y)), man(X), woman(Y), assert(marriage(X,Y)).

kuppapierosy(X) :- age(X,Y), Y > 17, write('OK').