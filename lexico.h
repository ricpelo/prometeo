#ifndef _LEXICO_H
#define _LEXICO_H

#include <ctype.h>
#include <dir.h>

#define LONG_BUFFER		2048

int EsAlfanum(int c);
int EsAlfa(int c);
extern unsigned int numLinea;
extern int token;
extern int numErrores;
extern char drive[MAXDRIVE],
			dir[MAXDIR],
			 file[MAXFILE],
			 ext[MAXEXT];

enum { VERBO1 = 1000, ADVERBIO1, NOMBRE1, ADJETIVO1, PREPOSICION1,
	   CONJUNCION1, PRONOMBRE1 };

#endif
