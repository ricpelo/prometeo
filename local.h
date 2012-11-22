#ifndef _LOCAL_H
#define _LOCAL_H

#include <stdio.h>
#define NUM_LOCALIDADES         300   	/* N£mero m ximo de localidades */

typedef struct Conexion {
	int direc;
	int destino;
	struct Conexion *siguiente;
} CONEXION;

typedef struct {
	char *descripcion;
	int pantalla;
	CONEXION *conexion;
} LOCAL;

extern LOCAL local[NUM_LOCALIDADES];   	/* Las localidades */
extern int numLocal;                  	/* N£mero actual de localidadades */

int InsertarLocalidad(int num, char *desc, int imagen, CONEXION *conex);
char *DescripcionLocalidad(int num);
int PantallaLocalidad(int num);
CONEXION *ConexionLocalidad(int num);
int ExisteLocalidad(int num);
void SalvarLocalidad(FILE *fp);
void CargarLocalidad(FILE *fp);

#endif
