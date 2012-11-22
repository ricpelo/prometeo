#ifndef _ETIQ_H
#define _ETIQ_H

#define NUM_ETIQUETAS	256				/* N£mero m ximo de etiquetas */

extern int etiquetas[NUM_ETIQUETAS];	/* Las etiquetas */

void InsertarEtiqueta(int num, int dir);
int DireccionEtiqueta(int num);

#endif