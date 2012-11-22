#ifndef _TABLA_C
#define _TABLA_C

#define TAM_TABLA   117

typedef struct Elemento {
	char *clave;
	char *info;
	struct Elemento *siguiente;
} ELEMENTO;

int InsertarElemento(char *cadena, char *inf);
ELEMENTO *DarElemento(char *cadena);
char *DarInfo(char *cadena);

#endif
