#ifndef _COD_RT_H
#define _COD_RT_H

/*#define TAM_CODIGO		10240*/

#include "Instruc.H"

typedef struct Codigo {
	char codop;
	int argumento;
} CODIGO;

extern unsigned long TAM_CODIGO;
extern CODIGO *codigo;
extern unsigned long pc;

#endif
