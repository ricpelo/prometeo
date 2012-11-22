#ifndef _OBJETO_H
#define _OBJETO_H

#include "Local.H"

#define NO_CREADO		(NUM_LOCALIDADES - 3)
#define ENCIMA			(NUM_LOCALIDADES - 2)
#define LLEVADO			(NUM_LOCALIDADES - 1)
#define ENCIMA_LLEVADO	(NUM_LOCALIDADES - 4)

typedef enum { T_NORMAL, T_CONTENEDOR, T_ROPA } TIPO_OBJETO;

struct Objeto {
	int codigo;
	TIPO_OBJETO tipo;
	int peso;
	int nombre;
	int adjetivo;
	char *descripcion;
	int inicial;
	int actual;
	int almacen;
	struct Objeto *siguiente;
};

typedef struct Objeto OBJETO;

int DarCodigoObjeto(OBJETO *o);
void FijarCodigoObjeto(OBJETO *o, int cod);
TIPO_OBJETO DarTipoObjeto(OBJETO *o);
void FijarTipoObjeto(OBJETO *o, TIPO_OBJETO t);
int DarPesoObjeto(OBJETO *o);
void FijarPesoObjeto(OBJETO *o, int p);
void DarNombreObjeto(OBJETO *o, int *nombre, int *adjetivo);
void FijarNombreObjeto(OBJETO *o, int nombre, int adjetivo);
char *DarDescripcionObjeto(OBJETO *o);
int DarInicialObjeto(OBJETO *o);
void FijarInicialObjeto(OBJETO *o, int inic);
int DarActualObjeto(OBJETO *o);
void FijarActualObjeto(OBJETO *o, int act);
int DarAlmacenObjeto(OBJETO *o);
void FijarAlmacenObjeto(OBJETO *o, int almac);
OBJETO *DarSiguienteObjeto(OBJETO *o);
void FijarSiguienteObjeto(OBJETO *o1, OBJETO *o2);

#endif
