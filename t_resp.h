#ifndef _T_RESP_H
#define _T_RESP_H

#include <stdio.h>
#include "Respuesta.H"

#define TABLA_RESPUESTA	117			   	/* Tama¤o vector de Tabla Hash */

extern RESPUESTA *tablaResp[TABLA_RESPUESTA];/* Vector de la tabla Hash */
extern int numResp;							 /* N§ actual de respuestas */

int InsertarRespuesta(int verb, int nomb, int dir);
RESPUESTA *DarRespuesta(int verb, int nomb);
void SalvarRespuesta(FILE *fp);
void CargarRespuesta(FILE *fp);

#endif