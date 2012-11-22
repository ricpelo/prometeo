#ifndef _ACCIONES_H
#define _ACCIONES_H

#include "L_Objeto.H"
#include "T_Objeto.H"
#include "Smix.H"

#define NUM_SONIDOS			16

extern SOUND *sonidos[NUM_SONIDOS];
extern int numSonidos;

void Emite(char *s);
void EmiteLn(char *s);
void EmiteEntero(char *s, int n);
void EmiteEnteroLn(char *s, int n);
void EmiteCadena(char *s1, char *s2);
void EmiteCadenaLn(char *s1, char *s2);
void EmiteCadenaError(char *cadena, char *valor);
void EmiteEnteroError(char *cadena, int valor);
void EmiteError(char *cadena);
int Acabar1(void);
int Acabar(void);
int Coger(int o);
int Dejar(int o);
int Poner(int o);
int Quitar(int o);
void Intercambiar(int o1, int o2);
int Meter(int c, int o);
int Sacar(int c, int o);
int SiguienteObjeto(int loc, int indic);
void ListaObj(int loc);
void ListaDe(int loc);
void Turnos(void);
void Inventario(void);
void Tecla(void);
void Diagnostico(void);
void SalvarSituacion(void);
int CargarSituacion(void);
void Salidas(void);
void Sonido(char *p, int loop);
void Silencio(char *p);
int Caminar(int loc, int direc);

#endif
