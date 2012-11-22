#include <alloc.h>
#include <string.h>
#include "Subrutina.H"
#include "Memoria.H"

/* DarNombreSubrutina():	Devuelve el nombre de la subrutina */
char *DarNombreSubrutina(SUBR *subrutina)
{
	return (subrutina != NULL) ? subrutina->nombre : NULL;
}


/*
	FijarNombreSubrutina():	Fija el nombre de una subrutina
	Recibe:					Un puntero al nombre
*/
void FijarNombreSubrutina(SUBR *subrutina, char *nomb)
{
	char *p = (char*) Memoria((long) strlen(nomb) + 1);
	strcpy(p, nomb);
	subrutina->nombre = p;
}


/* DarEtiquetaSubrutina():	Determina la etiqueta de comienzo de la
							subrutina */
int DarEtiquetaSubrutina(SUBR *subrutina)
{
	return (subrutina != NULL) ? subrutina->etiqueta : -1;
}


/*
	FijarEtiquetaSubrutina():	Fija la etiqueta de inicio de un subrutina
	Recibe:						Un puntero a la subrutina, y la etiqueta
*/
void FijarEtiquetaSubrutina(SUBR *subrutina, int etiq)
{
	subrutina->etiqueta = etiq;
}


/* DarSiguienteSubrutina():	Determina siguiente subrutina en tabla Hash */
SUBR *DarSiguienteSubrutina(SUBR *subrutina)
{
	return (subrutina != NULL) ? subrutina->siguiente : NULL;
}


/*
	FijarSiguienteSubrutina(): 	Fija siguiente subrutina en tabla Hash
	Recibe:				   		Un puntero a subrutina
*/
void FijarSiguienteSubrutina(SUBR *o1, SUBR *o2)
{
	o1->siguiente = o2;
}
