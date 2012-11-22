#ifndef _CODIGO_H
#define _CODIGO_H

/*#define TAM_CODIGO		10240*/
#define S_DIRECCION		0		/* Para el significado... */
#define S_ETIQUETA		1		/* ...del campo "signif" */

#include "Instruc.H"

typedef struct Codigo {
	int etiqueta;
	char codop;
	char signif;	  	/* Indica si el argumento es una dir. o una etiq. */
	int argumento;
} CODIGO;

extern unsigned long TAM_CODIGO;
extern CODIGO *codigo;
extern unsigned long despl;

#endif
