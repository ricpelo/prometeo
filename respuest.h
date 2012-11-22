#ifndef _RESPUEST_H
#define _RESPUEST_H

struct Respuesta {
	int verbo;
	int nombre;
	int direccion;
	struct Respuesta *siguiente;
};
typedef struct Respuesta RESPUESTA;

int DarDireccionRespuesta(RESPUESTA *r);
void FijarDireccionRespuesta(RESPUESTA *r, int dir);
RESPUESTA *DarSiguienteRespuesta(RESPUESTA *r);
void FijarSiguienteRespuesta(RESPUESTA *r1, RESPUESTA *r2);

#endif
