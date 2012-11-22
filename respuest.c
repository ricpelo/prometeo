#include <stdlib.h>
#include "Respuest.H"


/*
	DarDireccionRespuesta():	Devuelve la direcci¢n dentro del c¢digo
								de la primera instrucci¢n a ejecutar
								en respuesta a una determinada acci¢n
*/
int DarDireccionRespuesta(RESPUESTA *r)
{
	return (r != NULL) ? r->direccion : -1;
}


/*
	FijarDireccionRespuesta():	Fija la direcci¢n de la primera instrucci¢n
								a ejecutar en respuesta a una acci¢n dada
	Recibe:						La direcci¢n (un ¡ndice del vector de
								c¢digo)
*/


void FijarDireccionRespuesta(RESPUESTA *r, int dir)
{
	r->direccion = dir;
}


/* DarSiguienteRespuesta():	Determina siguiente respuesta en tabla Hash */
RESPUESTA *DarSiguienteRespuesta(RESPUESTA *r)
{
	return (r != NULL) ? r->siguiente : NULL;
}


/*
	FijarSiguienteRespuesta(): 	Fija siguiente respuesta en tabla Hash
	Recibe:				   		Un puntero a una respuesta
*/
void FijarSiguienteRespuesta(RESPUESTA *r1, RESPUESTA *r2)
{
	r1->siguiente = r2;
}
