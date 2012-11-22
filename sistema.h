#ifndef _SISTEMA_H
#define _SISTEMA_H

#include <stdio.h>
#define NUM_SISTEMA	60				/* N£mero m ximo de mensajes del sist */

extern char *sistema[NUM_SISTEMA];  /* Los mensajes del sistema */
extern int numSist;					/* N£mero actual de mensajes */

int CambiarSistema(int num, char *desc);
char *DescripcionSistema(int num);
void SalvarSistema(FILE *fp);
void CargarSistema(FILE *fp);

#endif