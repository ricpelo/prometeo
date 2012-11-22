#ifndef _PALABRA_H
#define _PALABRA_H

/*#define LONG_LEX	7 */					/* Long. del lexema, incluye '\0' */

typedef enum { T_VERBO, T_ADVERBIO, T_NOMBRE, T_ADJETIVO,
			   T_PREPOSICION, T_CONJUNCION, T_PRONOMBRE, T_NOMBRE_VERBO,
			   T_DIRECCION }
			   TIPO_PALABRA;

struct Palabra {
	int codigo;
	TIPO_PALABRA tipo;
	char *lexema;
	struct Palabra *sinonimos;
	struct Palabra *siguiente;
};
typedef struct Palabra PALABRA;

int DarCodigoPalabra(PALABRA *p);
void FijarCodigoPalabra(PALABRA *p, int cod);
TIPO_PALABRA DarTipoPalabra(PALABRA *p);
void FijarTipoPalabra(PALABRA *p, TIPO_PALABRA t);
char *DarLexemaPalabra(PALABRA *p);
void FijarLexemaPalabra(PALABRA *p, char *lex);
PALABRA *DarSinonimoPalabra(PALABRA *p);
void FijarSinonimoPalabra(PALABRA *p1, PALABRA *p2);
PALABRA *DarSiguientePalabra(PALABRA *p);
void FijarSiguientePalabra(PALABRA *p1, PALABRA *p2);

#endif
