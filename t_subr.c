#include <string.h>
#include <alloc.h>
#include "T_Subr.H"
#include "Subrutin.H"
#include "Etiq.H"
#include "Memoria.H"

SUBR *tablaSubr[TABLA_SUBRUTINA];

/* fHash():	funci¢n de asociaci¢n de la tabla hash */
static int fHash(char *nomb)
{
	int pos = 0;
	int i;

	for (i = 0; nomb[i]; i++)
		pos += nomb[i];

	return pos % TABLA_SUBRUTINA;
}


/*
	InsertarSubrutina():	Inserta una subrutina en la tabla de subrutinas
	Recibe:    				El nombre de la subrutina y su etiqueta
							correspondiente
	Devuelve:				1 si no hay errores; 0 en caso contrario
*/
int InsertarSubrutina(char *nomb, int etiq)
{
	SUBR *aux, *i;
	char *p;
	int pos, cod;
	int j;

	if (DarSubrutina(nomb) == NULL) {	/* Si no existe ya ese subrutina */

		if ((aux = (SUBR*) Memoria((long) sizeof(SUBR))) == NULL)
			return 0;

		p = (char*) Memoria((long) strlen(nomb) + 1);
		strcpy(p, nomb),
		aux->nombre = p;
		aux->etiqueta = etiq;
		aux->siguiente = NULL;

		/* B£squeda por nombre */
		pos = fHash(nomb);
		i = tablaSubr[pos];

		if (i == NULL)
			tablaSubr[pos] = aux;		 /* Actualizaci¢n del vector hash */
		else {

			while (i->siguiente != NULL)
				i = i->siguiente;

			i->siguiente = aux;
		}

		return 1;
	} else
		return 0;
}


/*
	DarSubrutina():	Devuelve un puntero a una cierta subrutina
	Recibe:			El nombre de la subrutina
*/
SUBR *DarSubrutina(char *nomb)
{
	SUBR *aux;
	int encontrado;

	aux = tablaSubr[fHash(nomb)];
	encontrado = 0;

	while (aux != NULL && !encontrado) {

		if (strcmp(aux->nombre, nomb) == 0)
			encontrado = 1;
		else
			aux = aux->siguiente;

	}

	return (encontrado) ? aux : NULL;
}


/*
	SalvarSubrutina():	Guarda dos enteros, correspondientes a las
						direcciones en las que est n almacenadas las
						subrutinas Process1 y Process2
	Recibe:				Un puntero al stream donde escribir
*/
void SalvarSubrutina(FILE *fp)
{
	SUBR *aux;
	int d, j = -1;

	if ((aux = DarSubrutina("Process1")) != NULL) {
		d = DireccionEtiqueta(DarEtiquetaSubrutina(aux));
		fwrite(&d, sizeof d, 1, fp);
	} else
		fwrite(&j, sizeof j, 1, fp);

	if ((aux = DarSubrutina("Process2")) != NULL) {
		d = DireccionEtiqueta(DarEtiquetaSubrutina(aux));
		fwrite(&d, sizeof d, 1, fp);
	} else
		fwrite(&j, sizeof j, 1, fp);

	if ((aux = DarSubrutina("Timeout")) != NULL) {
		d = DireccionEtiqueta(DarEtiquetaSubrutina(aux));
		fwrite(&d, sizeof d, 1, fp);
	} else
		fwrite(&j, sizeof j, 1, fp);

}
