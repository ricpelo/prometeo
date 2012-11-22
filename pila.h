#ifndef _PILA_H
#define _PILA_H

typedef struct Pila {
	int valor;
	struct Pila *siguiente;
} PILA;

int TopePila(PILA *p);
int SacaPila(PILA **p);
int EmpujaPila(PILA **p, int val);

#endif
