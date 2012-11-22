#ifndef _T_OBJETO_H
#define _T_OBJETO_H

#include <stdio.h>
#include "Objeto.H"

#define TABLA_OBJETO	117			   	/* Tama¤o vector de Tabla Hash */
#define INDICE_OBJETO	512			   	/* Tama¤o vector ¡ndice */


extern OBJETO *tablaObj[TABLA_OBJETO];   /* El vector de la tabla Hash */
extern OBJETO *indiceObj[INDICE_OBJETO]; /* Indice por c¢digo */
extern int numObj;						 /* N£mero actual de palabras */

int InsertarObjeto(TIPO_OBJETO tipoObj, int peso, int nomb, int adj,
				   char *desc, int inic, int act, int almac, int cod);
int CodigoObjeto(int nomb, int adj);
OBJETO *DarObjeto(int cod);
void SalvarObjeto(FILE *fp);
void CargarObjeto(FILE *fp);

#endif