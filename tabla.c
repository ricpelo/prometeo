#include <string.h>
#include <alloc.h>
#include "Tabla.H"
#include "Memoria.H"

ELEMENTO *tabla[TAM_TABLA];
int numElem = 0;


static int fHash(char *cod)
{
	int i, pos;

	for (i = pos = 0; cod[i]; i++)
		pos += cod[i];

	return pos % TAM_TABLA;
}


int InsertarElemento(char *cadena, char *inf)
{
	ELEMENTO *aux, *i;
	char *p;
	int pos, cod;
	int j;

	if ((aux = (ELEMENTO*) Memoria((long) sizeof(ELEMENTO))) == NULL)
		return 0;

	p = (char*) Memoria((long) strlen(cadena) + 1);
	strcpy(p, cadena),
	aux->clave = p;
	p = (char*) Memoria((long) strlen(inf) + 1);
	strcpy(p, inf);
	aux->info = p;
	aux->siguiente = NULL;

	/* B£squeda por nombre */
	pos = fHash(cadena);
	i = tabla[pos];

	if (i == NULL)
		tabla[pos] = aux;    /* Actualizaci¢n del vector hash */
	else {

		while (i->siguiente != NULL)
			i = i->siguiente;

		i->siguiente = aux;
	}

	numElem++;
	return 1;
}


ELEMENTO *DarElemento(char *cadena)
{
	ELEMENTO *aux;
	int encontrado;

	aux = tabla[fHash(cadena)];
	encontrado = 0;

	while (aux != NULL && !encontrado) {

		if (strcmp(cadena, aux->clave) == 0)
			encontrado = 1;
		else
			aux = aux->siguiente;

	}

	return (encontrado) ? aux : NULL;
}


char *DarInfo(char *cadena)
{
	ELEMENTO *e = DarElemento(cadena);

	return (e != NULL) ? e->info : NULL;
}

