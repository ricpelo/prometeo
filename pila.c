#include <alloc.h>
#include "Pila.H"
#include "Memoria.H"


/*
	TopePila():	Devuelve el contenido del tope de la pila, sin sacarlo
	Recibe:		La pila
	Devuelve:	El valor del tope de la pila (un entero)
*/
int TopePila(PILA *p)
{
	return (p != NULL) ? p->valor : -1;
}


/*
	SacaPila():	Extrae el valor del tope de la pila, borr ndolo de ella
	Recibe:		La pila
	Devuelve:	El valor del tope de la pila. Si la operaci¢n no ha tenido
				‚xito, el valor del resultado es impredecible
*/
int SacaPila(PILA **p)
{
	PILA *q;
	int salida;

	if (*p != NULL) {
		salida = TopePila(*p);
		q = *p;
		*p = (*p)->siguiente;
		farfree(q);
	}

	return salida;
}


/*
	EmpujaPila():	Inserta un valor en la pila, por encima de todos los
					dem s
	Recibe:			La pila y el valor a insertar en ella
	Devuelve:		1 si ha habido ‚xito; 0 en caso contrario
*/
int EmpujaPila(PILA **p, int val)
{
	PILA *q;

	if ((q = (PILA*) Memoria((long) sizeof(PILA))) == NULL)
		return 0;


	q->valor = val;
	q->siguiente = *p;
	*p = q;

	return 1;
}

