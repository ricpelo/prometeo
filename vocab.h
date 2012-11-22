#ifndef _VOCAB_H
#define _VOCAB_H

#include <stdio.h>
#include "Palabra.H"

#define TABLA_PALABRA	117			   	/* Tama¤o vector de tabla Hash */
#define INDICE_PALABRA	1024		   	/* Tama¤o vector ¡ndice */


extern PALABRA *tablaPal[TABLA_PALABRA];  /* El vector de la tabla Hash */
extern PALABRA *indicePal[INDICE_PALABRA];/* Indice por c¢digo */
extern int numPal;						  /* N£mero actual de palabras */

void InicializarVocab(void);
int InsertarVocab(TIPO_PALABRA tipoPal, char *lex, int cod);
int CodigoVocab(char *lex);
PALABRA *DarPalabraVocab(char *lex);
PALABRA *PrimeraVocab(int cod);
void SalvarVocab(FILE *fp);
void CargarVocab(FILE *fp);

#endif