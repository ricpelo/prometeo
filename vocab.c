#include <string.h>
#include <stdio.h>
#include <alloc.h>
#include "Memoria.H"
#include "Vocab.H"
#include "Palabra.H"
#include "Lexico.H"

PALABRA *tablaPal[TABLA_PALABRA];
PALABRA *indicePal[INDICE_PALABRA];
int numPal = 0;

/* fHash():	Funci¢n de asociaci¢n de la tabla hash */
static int fHash(char *lex)
{
	int pos = 0;
	int i;

	for (i = 0; lex[i]; i++)
		pos += lex[i];

	return pos % TABLA_PALABRA;
}


/*
	InsertarVocab():	Inserta una palabra en el vocabulario
	Recibe:    			tipo, lexema y c¢digo de la palabra
						(si ‚ste es == -1, se indica el siguiente c¢digo
						libre)
	Devuelve:			1 si no hay errores; 0 en caso contrario
*/
int InsertarVocab(TIPO_PALABRA tipoPal, char *lex, int cod)
{
	PALABRA *aux, *i;
	int pos;
	int j;

	if (CodigoVocab(lex) == -1) {	  	/* Si no existe ya esa palabra */

		if ((aux = (PALABRA*) Memoria((long) sizeof(PALABRA))) == NULL)
			return 0;

		if (cod == -1) {				/* Insertamos con un nuevo c¢digo */
			for (j = 0; j < INDICE_PALABRA && indicePal[j] != NULL; j++);
			if (j >= INDICE_PALABRA)
				return 0;
			else
				cod = j;
		}

		aux->codigo = cod;
		aux->tipo = tipoPal;
		aux->lexema = (char *) Memoria((long) (strlen(lex) + 1));
		strcpy(aux->lexema, lex);
		aux->sinonimos = NULL;
		aux->siguiente = NULL;

		/* B£squeda por lexema */
		pos = fHash(lex);
		i = tablaPal[pos];

		if (i == NULL)
			tablaPal[pos] = aux;			/* Actualizaci¢n del vector hash */
		else {

			while (i->siguiente != NULL)
				i = i->siguiente;

			i->siguiente = aux;
		}

		/* B£squeda por c¢digo */
		i = indicePal[cod];

		if (i == NULL) {
			indicePal[cod] = aux;			/* Actualizaci¢n del ¡ndice */
		} else {

			while (i->sinonimos != NULL)
				i = i->sinonimos;

			i->sinonimos = aux;
		}

		numPal++;
		return 1;
	} else
		return 0;
}


/*
	CodigoVocab():	Determina el c¢digo de una cierta palabra almacenada
	Recibe:	    	El lexema de la palabra
	Devuelve:		El c¢digo de dicha palabra, o -1 si no existe en el voc.
*/
int CodigoVocab(char *lex)
{
	PALABRA *aux;
	return ((aux = DarPalabraVocab(lex)) != NULL) ? aux->codigo : -1;
}


/*
	DarPalabra():	Devuelve un puntero a la direcci¢n de la palabra
	Recibe:			El lexema de la palabra
	Devuelve:		El puntero a la palabra, o NULL si no exist¡a
*/
PALABRA *DarPalabraVocab(char *lex)
{
	PALABRA *aux;
	int encontrado;

	aux = tablaPal[fHash(lex)];
	encontrado = 0;
	while (aux != NULL && !encontrado) {
		if (!strcmp(lex, aux->lexema))
			encontrado = 1;
		else
			aux = aux->siguiente;

	}

	return (encontrado) ? aux : NULL;
}

/*
	PrimeraVocab():	Busca la primera palabra que tenga un c¢digo determinado
	Recibe:			El c¢digo de la palabra
	Devuelve:		Un puntero a la palabra
*/
PALABRA *PrimeraVocab(int cod)
{
	return indicePal[cod];
}


/*
	SalvarVocab():	Salva el vocabulario en un stream
	Recibe:			El puntero al stream
*/
void SalvarVocab(FILE *fp)
{
	int i, j;
	PALABRA *p;

	fwrite(&numPal, sizeof numPal, 1, fp);

	for (i = 0; i < INDICE_PALABRA; i++) {
		p = indicePal[i];
		while (p != NULL) {
			fwrite(&p->codigo, sizeof p->codigo, 1, fp);
			fwrite(&p->tipo, sizeof p->tipo, 1, fp);

			for (j = 0; j < strlen(p->lexema); j++)
				(p->lexema)[j] ^= 219;

			fwrite(p->lexema, strlen(p->lexema) + 1, 1, fp);
			p = p->sinonimos;
		}
	}
}


/*
	CargarVocab():	Carga el vocabulario desde un stream
	Recibe:			El puntero al stream
*/
void CargarVocab(FILE *fp)
{
	int i, j, n;
	PALABRA p;
	char buffer[LONG_BUFFER];
	int c;

	fread(&n, sizeof n, 1, fp);			/* N§ de palabras almacenadas */

	for (i = 0; i < n; i++) {
		fread(&p.codigo, sizeof p.codigo, 1, fp);
		fread(&p.tipo, sizeof p.tipo, 1, fp);
		j = 0;

		while ((c = getc(fp)) != '\0')
			buffer[j++] = c;

		buffer[j] = '\0';

		p.lexema = (char *) Memoria((long) (strlen(buffer) + 1));
		strcpy(p.lexema, buffer);

		for (j = 0; j < strlen(p.lexema); j++)
			(p.lexema)[j] ^= 219;

		InsertarVocab(p.tipo, p.lexema, p.codigo);
	}

}
