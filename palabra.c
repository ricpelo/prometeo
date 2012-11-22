#include <string.h>
#include "Palabra.H"
#include "Memoria.H"


/*	DarCodigoPalabra():	Devuelve el c¢digo de una palabra */
int DarCodigoPalabra(PALABRA *palabra)
{
	return (palabra != NULL) ? palabra->codigo : -1;
}


/*
	FijarCodigoPalabra():	Fija el c¢digo de una palabra
	Recibe:					El c¢digo
*/
void FijarCodigoPalabra(PALABRA *palabra, int cod)
{
	palabra->codigo = cod;
}


/* DarTipoPalabra():	Devuelve el tipo de la palabra */
TIPO_PALABRA DarTipoPalabra(PALABRA *palabra)
{
	return (palabra != NULL ) ? palabra->tipo : -1;
}


/*
	FijarTipoPalabra():	Fija el tipo de una palabra
	Recibe:				El tipo
*/
void FijarTipoPalabra(PALABRA *palabra, TIPO_PALABRA t)
{
	palabra->tipo = t;
}


/* DarLexemaPalabra():	Determina el lexema de una palabra */
char *DarLexemaPalabra(PALABRA *palabra)
{
	return (palabra != NULL) ? palabra->lexema : NULL;
}


/*
	FijarLexemaPalabra():	Fija el lexema de una palabra
	Recibe:					Un puntero al lexema
*/
void FijarLexemaPalabra(PALABRA *palabra, char *lex)
{
	palabra->lexema = (char *) Memoria((long) (strlen(lex) + 1));
	strcpy(palabra->lexema, lex);
}


/* DarSinonimoPalabra():	Devuelve el siguiente sin¢nimo de una palabra */
PALABRA *DarSinonimoPalabra(PALABRA *palabra)
{
	return (palabra != NULL) ? palabra->sinonimos : NULL;
}


/*
	FijarSinonimoPalabra():	Fija el siguiente sin¢nimo de una palabra
	Recibe:					Un puntero a palabra
*/
void FijarSinonimoPalabra(PALABRA *p1, PALABRA *p2)
{
	p1->sinonimos = p2;
}


/* DarSiguientePalabra():	Determina siguiente palabra en tabla Hash */
PALABRA *DarSiguientePalabra(PALABRA *palabra)
{
	return (palabra != NULL) ? palabra->siguiente : NULL;
}


/*
	FijarSiguientePalabra(): 	Fija siguiente palabra en tabla Hash
	Recibe:				   		Un puntero a palabra
*/
void FijarSiguientePalabra(PALABRA *p1, PALABRA *p2)
{
	p1->siguiente = p2;
}
