%{
#include "Sistema.H"
#include "Bandera.H"
#include "Local.H"
#include "Acciones.H"
#include "T_Resp.H"
#include "Runtime.H"
#include "Input.H"
#include "Interpre.H"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <alloc.h>
#include "Memoria.H"
#include "Vocab.H"
int yylex(void);
int yyerror(char *s);
void EjecutaSentencia(void);
int BuscaRespuesta(int verb, int nomb);
void BanderasObjeto(void);
%}

%token VERBO NOMBRE ADJETIVO ADVERBIO PREPOSICION CONJUNCION PRONOMBRE
%token NOMBRE_VERBO DIRECCION

%%

inicial : { syntaxError = terminal = hecho = 0; bandera[33] = bandera[34] =
			bandera[35] = bandera[36] = bandera[43] = bandera[44] =
			bandera[45] = bandera[48] = bandera[49] = bandera[50] = -1; }
		  entrada
		;

entrada : entrada separador sentencia
		| sentencia
		;

separador : CONJUNCION
		  | '.'
		  | ','
		  | ';'
		  ;

sentencia : frase { if (!syntaxError) EjecutaSentencia(); }
		  | frase VERBO verbos { syntaxError = 1; yyerror("syntax error"); }
		  ;

verbos : verbos VERBO
	   | verbos NOMBRE_VERBO
	   | verbos DIRECCION
	   |
	   ;

frase : VERBO { hecho = 0; bandera[33] = $1; }
			preposicion nombre cadena adverbio
	  | NOMBRE_VERBO { hecho = 0; bandera[33] = $1; }
			preposicion nombre cadena adverbio
	  | DIRECCION { hecho = 0; bandera[33] = $1; }
			preposicion nombre cadena adverbio
	  | NOMBRE
		{ if (bandera[33] >= 0) {
			  bandera[34] = $1;
			  hecho = 0;
		  }
		} adjetivo preposicion nombre2 cadena adverbio
	  | PRONOMBRE
		{ if (bandera[33] >= 0) hecho = 0; }
		adjetivo preposicion nombre2 cadena adverbio
	  ;

cadena : '\"' sentencia_cadena '\"'
	   |
	   ;

sentencia_cadena : VERBO    { bandera[48] = $1; } nombre_cadena
				 | NOMBRE   { bandera[49] = $1; } adjetivo_cadena
				 | ADJETIVO { bandera[50] = $1; }
				 |
				 ;

nombre_cadena : NOMBRE   { bandera[49] = $1; } adjetivo_cadena
			  | ADJETIVO { bandera[50] = $1; }
			  |
			  ;

adjetivo_cadena : ADJETIVO { bandera[50] = $1; }
				|
				;

preposicion : PREPOSICION { bandera[43] = $1; }
			|
			;

nombre : NOMBRE { bandera[34] = $1; }
		 adjetivo preposicion nombre2
	   | PRONOMBRE preposicion nombre2
	   | { if (!terminal) bandera[34] = -1; }
	   | NOMBRE_VERBO { bandera[34] = $1; }
		 adjetivo preposicion nombre2
	   | DIRECCION { bandera[34] = $1; }
		 adjetivo preposicion nombre2
	   ;

adjetivo : ADJETIVO	{ bandera[35] = $1; }
		 |
		 ;

nombre2 : NOMBRE { bandera[44] = $1; } adjetivo2
		| PRONOMBRE
		|
		;

adjetivo2 : ADJETIVO { bandera[45] = $1; }
		  |
		  ;

adverbio : ADVERBIO { bandera[36] = $1; }
		 |
		 ;

%%

int yyerror(char *s)
{
	if (s != NULL && !tiempo) {	/* Si ha habido un tiempo muerto, nada */
		Emite(sistema[6]); 				/* "No te entiendo" */
		syntaxError = 1;
		return 0;
	}

	return 1;
}

void EjecutaSentencia(void)
{
	CONEXION *co;
	int verb, nomb;
	int oscuro = bandera[0] != 0;
	int luzAusente = DarActualObjeto(DarObjeto(0)) != bandera[38];
	int sent = 0;

	bandera[31]++;				/* Incrementamos el n§ de turnos */

	if (bandera[5] > 0)
		bandera[5]--;
	if (bandera[6] > 0)
		bandera[6]--;
	if (bandera[7] > 0)
		bandera[7]--;
	if (bandera[8] > 0)
		bandera[8]--;

	if (oscuro) {
		if (bandera[9] > 0)
			bandera[9]--;
		if (bandera[10] > 0 && luzAusente)
			bandera[10]--;
	}

	verb = bandera[33];
	nomb = bandera[34];
	BanderasObjeto();
	co = ConexionLocalidad(bandera[38]);	/* 38 = Localidad actual */

	/* Respuestas de mayor a menor prioridad */
	sent = BuscaRespuesta(-1, -1) || sent; 	 	/* Respuestas "* *" */
	if (nomb >= 0)
		sent = BuscaRespuesta(-1, nomb) || sent;  /* Respuestas "* NOMBRE" */
	if (verb >= 0)
		sent = BuscaRespuesta(verb, -1) || sent; 	/* Respuestas "VERBO *" */
	if (verb >= 0 && nomb >= 0)
		sent = BuscaRespuesta(verb, nomb) || sent; /* Respuestas "VERBO NOMBRE" */
	if (verb >= 0)
		sent = BuscaRespuesta(verb, -2) || sent;  	/* Respuestas "VERBO _" */
	if (nomb >= 0)
		sent = BuscaRespuesta(-2, nomb) || sent;   /* Respuestas "_ NOMBRE" */
	sent = BuscaRespuesta(-2, -2) || sent;	   		/* Respuestas "_ _" */

	if (hecho != 1) {
		/* Comprobar si se ha introducido una acci¢n de movimiento */
		while (co != NULL && co->direc != verb)
			co = co->siguiente;

		if (co != NULL) {  				/* La conexion existe */
			if (!ExisteLocalidad(co->destino))
				EmiteError("ERROR: La localidad no existe");
			else {
				bandera[38] = co->destino;	/* Mueve el jugador al destino */
				DescribeSituacion();		/* Describe nueva localidad */
			}
		} else {
			if (!sent) {
				if (DarTipoPalabra(PrimeraVocab(verb)) == T_DIRECCION)
					Emite(sistema[21]);		/* "No puedo ir en esa direcci¢n" */
				else
					Emite(sistema[8]);	/* "No puedes hacer eso" */
			}
		}
	}

}


/*
	BuscaRespuesta():	Ejecuta las acciones correspondientes para una
						cierta respuesta
	Recibe:				El verbo y el nombre de la respuesta a buscar
	Devuelve:			1 si se ha encontrado una respuesta v lida; 0 en otro
						caso
*/
int BuscaRespuesta(int verb, int nomb)
{
	int salida = 0;
	RESPUESTA *resp;

	/* La respuesta s¢lo da lugar a acciones cuando el interruptor "hecho"
	   est  apagado */
	if ((resp = DarRespuesta(verb, nomb)) != NULL && !hecho) {
		salida = 1;
		EmpujaPila(&direcciones, pc);
		pc = resp->direccion;
		Traductor();
	}

	return salida;
}


/*
	BanderasObjeto():	Actualiza las banderas relacionadas con la
						informaci¢n del objeto actualmente referido
*/
void BanderasObjeto(void)
{
	int o = CodigoObjeto(bandera[34], bandera[35]);
	OBJETO *obj = DarObjeto(o);
	int tipoObj;
	int valor, nombre, adjetivo;

	if (obj == NULL) {
		valor = Siguiente(ENCIMA_LLEVADO, 0);
		DarNombreObjeto(DarObjeto(valor), &nombre, &adjetivo);
		while (valor != -1 && nombre != bandera[34]) {
			valor = Siguiente(ENCIMA_LLEVADO, 1);
			DarNombreObjeto(DarObjeto(valor), &nombre, &adjetivo);
		}

		if (valor == -1) {
			valor = Siguiente(bandera[38], 0);
			DarNombreObjeto(DarObjeto(valor), &nombre, &adjetivo);
			while (valor != -1 && nombre != bandera[34]) {
				valor = Siguiente(bandera[38], 1);
				DarNombreObjeto(DarObjeto(valor), &nombre, &adjetivo);
			}

			if (valor != -1)
				obj = DarObjeto(valor);

		} else
			obj = DarObjeto(valor);
	}

	tipoObj = DarTipoObjeto(obj);
	bandera[54] = DarActualObjeto(obj);	/* Localidad actual */
	bandera[55] = DarPesoObjeto(obj);	/* Peso del objeto */
	bandera[56] = tipoObj == T_CONTENEDOR;
	bandera[57] = tipoObj == T_ROPA;
	bandera[58] = DarCodigoObjeto(obj);
}


int Siguiente(int loc, int indic)
{
	static LOBJETO *sgteObjeto = NULL;
	static LOBJETO *inicial = NULL;
	LOBJETO *p, *q;

	if (indic == 0) {
		p = inicial;

		while (p != NULL) {
			q = p;
			p = p->siguiente;
			farfree(q);
		}

		sgteObjeto = inicial = SiguienteAux(localObj[loc]);

	} else if (sgteObjeto != NULL)
		sgteObjeto = sgteObjeto->siguiente;

	return (sgteObjeto == NULL) ? -1 : sgteObjeto->codigo;
}


LOBJETO *SiguienteAux(LOBJETO *org)
{
	LOBJETO *dest;

	if (org == NULL)
		return NULL;
	else {
		dest = (LOBJETO*) Memoria((long) sizeof (LOBJETO));
		dest->codigo = org->codigo;
		dest->siguiente = SiguienteAux(org->siguiente);
		return dest;
	}
}
