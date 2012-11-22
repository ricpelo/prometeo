#ifndef _L_OBJETO_H
#define _L_OBJETO_H

#include "Local.H"

typedef struct LObjeto {
	int codigo;						/* El objeto c¢digo est  aqu¡ */
	struct LObjeto *siguiente;		/* Enlace para la lista */
} LOBJETO;

extern LOBJETO *localObj[NUM_LOCALIDADES];	/* El vector de listas */

int MeteObjetoLocal(int cod, int local);
void SacaObjetoLocal(int cod, int local);
void PasaObjetoLocal(int cod, int local);
void MueveObjeto(int cod, int local);

#endif