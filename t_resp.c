#include <string.h>
#include <alloc.h>
#include <stdio.h>
#include <math.h>
#include "T_Resp.H"
#include "Memoria.H"

RESPUESTA *tablaResp[TABLA_RESPUESTA];
int numResp = 0;

/* fHash():	Funci¢n de asociaci¢n de la tabla hash */
static int fHash(int verb, int nomb)
{
	int x = verb + nomb;
	return abs(x) % TABLA_RESPUESTA;
}


/*
	InsertarRespuesta():	Inserta una respuesta en la tabla de
							respuestas
	Recibe:					Respuesta (verbo y nombre) y direcci¢n
							de primera instrucci¢n a ejecutar en el
							vector de c¢digo
	Devuelve:				1 si no hay errores; 0 en caso contrario
*/
int InsertarRespuesta(int verb, int nomb, int dir)
{
	RESPUESTA *aux, *i;
	char *p;
	int pos, cod;
	int j;

	/* Si no existe ya esa respuesta */
	if (DarRespuesta(verb, nomb) == NULL) {

		if ((aux = (RESPUESTA*) Memoria((long) sizeof(RESPUESTA))) == NULL)
			return 0;

		aux->verbo = verb;
		aux->nombre = nomb;
		aux->direccion = dir;
		aux->siguiente = NULL;

		/* B£squeda por nombre y adjetivo */
		pos = fHash(verb, nomb);
		i = tablaResp[pos];

		if (i == NULL)
			tablaResp[pos] = aux;		 /* Actualizaci¢n del vector hash */
		else {

			while (i->siguiente != NULL)
				i = i->siguiente;

			i->siguiente = aux;
		}

		numResp++;
		return 1;
	} else
		return 0;
}


/*
	DarRespuesta():	Devuelve la respuesta dada para ese verbo y nombre
	Recibe:	    	Verbo y nombre de la respuesta
	Devuelve:		El puntero a la respuesta, o NULL si no existe
*/
RESPUESTA *DarRespuesta(int verb, int nomb)
{
	RESPUESTA *aux;
	int encontrado;

	aux = tablaResp[fHash(verb, nomb)];
	encontrado = 0;

	while (aux != NULL && !encontrado) {

		if (aux->verbo == verb && aux->nombre == nomb)
			encontrado = 1;
		else
			aux = aux->siguiente;

	}

	return (encontrado) ? aux : NULL;
}


/*
	SalvarRespuesta():	Salva la tabla de respuestas en un stream
	Recibe:				El puntero al stream
*/
void SalvarRespuesta(FILE *fp)
{
	int i;
	RESPUESTA *r;

	fwrite(&numResp, sizeof numResp, 1, fp);
	for (i = 0; i < TABLA_RESPUESTA; i++) {
		r = tablaResp[i];
		while (r != NULL) {
			fwrite(&r->verbo, sizeof r->verbo, 1, fp);
			fwrite(&r->nombre, sizeof r->nombre, 1, fp);
			fwrite(&r->direccion, sizeof r->direccion, 1, fp);
			r = r->siguiente;
		}
	}
}


/*
	CargarRespuesta():	Carga la tabla de respuestas desde un stream
	Recibe:				El puntero al stream
*/
void CargarRespuesta(FILE *fp)
{
	int i, n;
	RESPUESTA r;

	fread(&n, sizeof n, 1, fp);			/* N§ de respuestas almacenadas */

	for (i = 0; i < n; i++) {
		fread(&r.verbo, sizeof r.verbo, 1, fp);
		fread(&r.nombre, sizeof r.nombre, 1, fp);
		fread(&r.direccion, sizeof r.direccion, 1, fp);
		InsertarRespuesta(r.verbo, r.nombre, r.direccion);
	}
}

