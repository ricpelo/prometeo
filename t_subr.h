#ifndef _T_SUBR_H
#define _T_SUBR_H

#include <stdio.h>
#include "Subrutin.H"

#define TABLA_SUBRUTINA		117  	/* Tama¤o vector de Tabla Hash */


extern SUBR *tablaSubr[TABLA_SUBRUTINA];/* El vector de la tabla Hash */

int InsertarSubrutina(char *nomb, int etiq);
SUBR *DarSubrutina(char *nomb);
void SalvarSubrutina(FILE *fp);

#endif