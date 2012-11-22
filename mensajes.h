#ifndef _MENSAJES_H
#define _MENSAJES_H

#define TABLA_MENSAJES			117

struct Mensaje {
	int codigo;
	char *texto;
	struct Mensaje *siguiente;
};

typedef struct Mensaje MENSAJE;

char *DarTextoMensaje(MENSAJE *m);
MENSAJE *DarSiguienteMensaje(MENSAJE *m);

#endif