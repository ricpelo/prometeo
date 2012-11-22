#include <string.h>
#include <alloc.h>
#include "Mensajes.H"
#include "T_Mens.H"
#include "Lexico.H"						/* Para LONG_BUFFER */
#include "Memoria.H"

MENSAJE *tablaMens[TABLA_MENSAJES];
int numMens = 0;

/* fHash():	funci¢n de asociaci¢n de la tabla hash */
static int fHash(int cod)
{
	return cod % TABLA_MENSAJES;
}


/*
	BuscarMensaje():	Busca un mensaje dada su descripci¢n
	Recibe:    		    La descripci¢n
	Devuelve:			El n£mero del mensaje si exist¡a. -1 en caso contrario
*/
int BuscarMensaje(char *desc)
{
	int i;

	for (i = 0; i < numMens; i++)
		if (strcmp(DescripcionMensaje(i), desc) == 0)
			return i;

	return -1;
}


/*
	InsertarMensaje():	Inserta un mensaje
	Recibe:    			El n£mero y la descripci¢n
	Devuelve:			1 si se ha insertado con ‚xito. 0 en caso contrario
*/
int InsertarMensaje(int num, char *desc)
{
	MENSAJE *aux, *i;
	char *p;
	int pos, cod;
	int j;

	if ((aux = (MENSAJE*) Memoria((long) sizeof(MENSAJE))) == NULL)
		return 0;

	p = (char*) Memoria((long) strlen(desc) + 1);
	strcpy(p, desc),
	aux->texto  = p;
	aux->codigo = num;
	aux->siguiente = NULL;

	/* B£squeda por nombre */
	pos = fHash(num);
	i = tablaMens[pos];

	if (i == NULL)
		tablaMens[pos] = aux;	 /* Actualizaci¢n del vector hash */
	else {

		while (i->siguiente != NULL)
			i = i->siguiente;

		i->siguiente = aux;
	}

	numMens++;
	return 1;
}

MENSAJE *DarMensaje(int num)
{
	MENSAJE *aux;
	int encontrado;

	aux = tablaMens[fHash(num)];
	encontrado = 0;

	while (aux != NULL && !encontrado) {

		if (aux->codigo == num)
			encontrado = 1;
		else
			aux = aux->siguiente;

	}

	return (encontrado) ? aux : NULL;
}


/*
	DescripcionMensaje():	Devuelve un puntero a la descripci¢n del mensaje
	Recibe:    	    		El n£mero del mensaje
*/
char *DescripcionMensaje(int num)
{
	return (num >= 0 && num < numMens) ? DarTextoMensaje(DarMensaje(num)) :
			NULL;
}


/*
	SalvarMensaje():	Salva la tabla de mensajes en un stream
	Recibe:				Un puntero al stream
*/
void SalvarMensaje(FILE *fp)
{
	int i, j;
	MENSAJE *m;
	char *s;

	fwrite(&numMens, sizeof numMens, 1, fp);
	for (i = 0; i < TABLA_MENSAJES; i++) {
		m = tablaMens[i];
		while (m != NULL) {
			fwrite(&m->codigo, sizeof m->codigo, 1, fp);
			s = m->texto;

			for (j = 0; j < strlen(s); j++)
				s[j] ^= 219;

			fwrite(s, strlen(s) + 1, 1, fp);
			m = m->siguiente;
		}
	}

}


/*
	CargarMensaje():	Carga la tabla de mensajes desde un stream
	Recibe:				Un puntero al stream
*/
void CargarMensaje(FILE *fp)
{
	int i, j, k, n;
	int p;
	int c;
	char *s, buffer[LONG_BUFFER];

	fread(&n, sizeof n, 1, fp);			/* N§ de mensajes almacenados */

	for (i = 0; i < n; i++) {
		fread(&k, sizeof i, 1, fp);
		j = 0;

		while ((c = getc(fp)) != '\0')
			buffer[j++] = c;

		buffer[j] = '\0';
		s = (char*) Memoria((long) strlen(buffer) + 1);
		strcpy(s, buffer);

		for (p = 0; p < strlen(s); p++)
			s[p] ^= 219;

		InsertarMensaje(k, s);
	}
}
