#ifndef _RUNTIME_H
#define _RUNTIME_H

#include "Pila.H"

extern int terminal;
extern int timeout;
extern int pesoTotal;
extern int hecho;
extern unsigned long pc;
extern PILA *direcciones;
extern int syntaxError;
void DescribeSituacion(void);
void Traductor(void);

#endif
