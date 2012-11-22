#ifndef _SUBRUTIN_H
#define _SUBRUTIN_H

struct Subr {
	char *nombre;
	int etiqueta;
	struct Subr *siguiente;
};
typedef struct Subr SUBR;

char *DarNombreSubrutina(SUBR *f);
void FijarNombreSubrutina(SUBR *f, char *nomb);
int DarEtiquetaSubrutina(SUBR *f);
void FijarEtiquetaSubrutina(SUBR *f, int etiq);
SUBR *DarSiguienteSubrutina(SUBR *f);
void FijarSiguienteSubrutina(SUBR *f1, SUBR *f2);

#endif
