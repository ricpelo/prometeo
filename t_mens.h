#ifndef _TMENS_H
#define _TMENS_H

#include "Mensajes.H"
#include <stdio.h>

extern MENSAJE *tablaMens[TABLA_MENSAJES];	/* Los mensajes */
extern int numMens;							/* N£mero actual de mensajes */

int InsertarMensaje(int num, char *desc);
int BuscarMensaje(char *desc);
MENSAJE *DarMensaje(int num);
char *DescripcionMensaje(int num);
void SalvarMensaje(FILE *fp);
void CargarMensaje(FILE *fp);

#endif
