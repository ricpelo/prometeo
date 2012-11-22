#include <string.h>
#include <alloc.h>
#include "Mensajes.H"
#include "Lexico.H"						/* Para LONG_BUFFER */

/*
	DarTextoMensaje()

*/

char *DarTextoMensaje(MENSAJE *mensaje)
{
	return mensaje->texto;
}


/*
	DarSiguienteMensaje()

*/

MENSAJE *DarSiguienteMensaje(MENSAJE *mensaje)
{
	return mensaje->siguiente;
}
