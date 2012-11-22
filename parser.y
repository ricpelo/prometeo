%{
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <alloc.h>
#include "Lexico.H"
#include "T_Mens.H"
#include "Sistema.H"
#include "Vocab.H"
#include "T_Objeto.H"
#include "Local.H"
#include "Etiq.H"
#include "T_Resp.H"
#include "Codigo.H"
#include "T_Subr.H"
#include "yydisp.h"

int eval;
int siguiente;
char *cadena;               /* Lleva la £ltima palabra para los sin¢nimos */
int yyparse(void);
int yylex(void);
void Error(char *);
void AsignaEti(int etiq, int inc);
void AsignaCod(char cod, int inc);
void AsignaArg(int arg, int inc);
void AsignaSig(char sig, int inc);
%}

%union {
	int valor;
	unsigned long linea;
	char *lex;
	TIPO_PALABRA tipoPal;
	TIPO_OBJETO tipoObj;
	CONEXION *conex;
}

%token MENSAJES NUM ID CADENA VOCABULARIO VERBO ADVERBIO NOMBRE ADJETIVO
%token PREPOSICION CONJUNCION PRONOMBRE OBJETOS DESC CONTENEDOR ROPA VERBO1
%token ADVERBIO1 NOMBRE1 ADJETIVO1 VERB1 NOMB1 PREPOSICION1 CONJUNCION1
%token PRONOMBRE1 PESO LOCALIDADES INICIAL IMAGEN IMAGEN_CICLO BANDERA ASIGNA
%token MAY MEN MAI MEI EQU NEQ LLAMA SUBRUTINA RETORNO MIENTRAS NOT AND OR
%token TRUE FALSE SI SINO ENTONCES SISTEMA ESTA ESTAMAY ESTAMEN PRESENTE
%token PUESTO1 ENCIMA1 LLEVADO1 ESTAEN ADJET1 ADVERB PREPOS NOMB2 ADJET2
%token ACABAR1 COGER DEJAR PONER QUITAR CREAR DESTRUIR ACABAR TURNOS HECHO
%token COLOCAR INTERCAMBIAR METER SACAR AUTOC AUTOD AUTOP AUTOQ AUTOM AUTOS
%token LISTAOBJ LISTADE INVEN MENSIS LLEVADO2 NO_CREADO1 TECLA
%token PAUSA AUSENTE DESCRIBE MUEVE LOCAL NO_HECHO DIAG IMPRIME ANALIZA
%token OBJETO SALVAR CARGAR BORRAR ALEATORIO SALIDAS PRIMERO SIGUIENTE
%token CONTENEDOR1 ROPA1 CURSOR POSX POSY SALTOLINEA SONIDO EVALUAR CASO
%token DEFECTO SILENCIO SONIDO_CICLO CAMINAR VENTANA COLOR FONDO LISTACONT
%token NOMBRE_VERBO NOMBRE_VERBO1 DIRECCION DIRECCION1 DIRECC1 FUENTE

%type <valor>   NUM BANDERA verbo nombre adjetivo
%type <lex>     ID CADENA VERBO1 ADVERBIO1 NOMBRE1 ADJETIVO1 PREPOSICION1
%type <lex>     CONJUNCION1 PRONOMBRE1 NOMBRE_VERBO1 DIRECCION1
%type <lex>     nombre_verbo verbo1
%type <tipoPal> tipo_palabra
%type <tipoObj> tipo_objeto
%type <conex>   conexiones lista_conexiones conexion

%left OR
%left AND
%left EQU NEQ
%left MAY MEN MAI MEI
%left '+' '-'
%left '*' '/' '%'
%left NOT ESTA ESTAMAY ESTAMEN PRESENTE AUSENTE PUESTO1 LLEVADO1 ADJET1
	  VERB1 NOMB1 ADVERB PREPOS NOMB2 ADJET2 ACABAR1 LOCAL MENOSU OBJETO
	  ESTAEN ANALIZA ALEATORIO PRIMERO SIGUIENTE PESO CONTENEDOR1 ROPA1
	  POSX POSY CAMINAR

%%

aventura : lista_secciones
		 ;

lista_secciones : lista_secciones seccion
				| seccion
				| lista_secciones error
				| error
				;

seccion : vocabulario
		| localidades
		| objetos
		| mens_sistema
		| respuesta
		| subrutina
		;

vocabulario : VOCABULARIO '{' lista_decl_palabras lla_ce
			| error '{' lista_decl_palabras lla_ce
			;

lista_decl_palabras : lista_decl_palabras palabra
					| palabra
					;

palabra : tipo_palabra lista_palabras pyc
		| error { $<tipoPal>$ = T_DIRECCION; } lista_palabras pyc
		;

tipo_palabra : VERBO        { $$ = T_VERBO; }
			 | ADVERBIO     { $$ = T_ADVERBIO; }
			 | NOMBRE       { $$ = T_NOMBRE; }
			 | ADJETIVO     { $$ = T_ADJETIVO; }
			 | PREPOSICION  { $$ = T_PREPOSICION; }
			 | CONJUNCION   { $$ = T_CONJUNCION; }
			 | PRONOMBRE    { $$ = T_PRONOMBRE; }
			 | NOMBRE_VERBO { $$ = T_NOMBRE_VERBO; }
			 | DIRECCION	{ $$ = T_DIRECCION; }
			 ;

lista_palabras : ID
				 { cadena = $1;
				   if (!InsertarVocab($<tipoPal>0, $1, -1))
					   Error("No hay memoria o palabra ya existente");
				   $<tipoPal>$ = $<tipoPal>0;
				 }
				 sgte_palabra
			   ;

sgte_palabra : ',' ID
			   { cadena = $2;
				 if (!InsertarVocab($<tipoPal>0, $2, -1))
					 Error("No hay memoria o palabra ya existente");
				 $<tipoPal>$ = $<tipoPal>0;
			   }
			   sgte_palabra
			 | EQU '{'
			   { $<tipoPal>$ = $<tipoPal>0; }
			   lista_sinonimos lla_ce { $<tipoPal>$ = $<tipoPal>0; }
			   sgte_palabra
			 |
			 ;

lista_sinonimos : lista_sinonimos ','
				  { $<tipoPal>$ = $<tipoPal>0; }
				  sinonimo
				| sinonimo
				;

sinonimo : ID
		   { if (!InsertarVocab($<tipoPal>0, $1, CodigoVocab(cadena)))
				 Error("No hay memoria o palabra ya existente");
		   }
		 ;

localidades : LOCALIDADES '{' lista_localidades lla_ce
			| error '{' lista_localidades lla_ce
			;

lista_localidades : lista_localidades localidad pyc
				  | localidad pyc
				  ;

localidad : NUM ':' CADENA pantalla conexiones
			{ if (!InsertarLocalidad($1, $3, $<valor>4, $5))
				  Error("No hay memoria o localidad ya existente"); }
		  | NUM ':' CADENA error
			{ if (!InsertarLocalidad($1, $3, 0, NULL))
				  Error("No hay memoria o localidad ya existente"); }
		  ;

pantalla : IMAGEN CADENA
		   { int m = gen_mens();
			 InsertarMensaje(m, $2); $<valor>$ = m; }
		 | { $<valor>$ = -1; }
		 ;

conexiones : '{' lista_conexiones lla_ce { $$ = $2; }
		   | { $$ = NULL; }
		   ;

lista_conexiones : conexion ',' lista_conexiones
				   { CONEXION *p;
					 ($1)->siguiente = $3;
					 p = $3;
					 while (p != NULL && p->direc != ($1)->direc)
						p = p->siguiente;
					 if (p != NULL)
						Error("Conexi¢n repetida");
					 $$ = $1; }
				 | conexion { $$ = $1; }
				 ;

conexion : VERBO1 ':' NUM
		   { CONEXION *c;
			 if ((c = (CONEXION*) farmalloc((long) sizeof(CONEXION))) == NULL)
				Error("No hay memoria para insertar conexiones");
			 else {
				c->direc = CodigoVocab($1); c->destino = $3;
				c->siguiente = NULL; $$ = c;
				printf("AVISO EN LINEA %u:\nSe usan palabras no "
					   "\"direccion\" en conexiones\n", numLinea);
			 }
		   }
		 | NOMBRE_VERBO1 ':' NUM
		   { CONEXION *c;
			 if ((c = (CONEXION*) farmalloc((long) sizeof(CONEXION))) == NULL)
				Error("No hay memoria para insertar conexiones");
			 else {
				c->direc = CodigoVocab($1); c->destino = $3;
				c->siguiente = NULL; $$ = c;
				printf("AVISO EN LINEA %u:\nSe usan palabras no "
					   "\"direccion\"en conexiones\n", numLinea);

			 }
		   }
		 | DIRECCION1 ':' NUM
		   { CONEXION *c;
			 if ((c = (CONEXION*) farmalloc((long) sizeof(CONEXION))) == NULL)
				Error("No hay memoria para insertar conexiones");
			 else {
				c->direc = CodigoVocab($1); c->destino = $3;
				c->siguiente = NULL; $$ = c;
			 }
		   }
		 ;

objetos : OBJETOS '{' lista_objetos lla_ce
		| error '{' lista_objetos lla_ce
		;

lista_objetos : lista_objetos objeto pyc
			  | objeto pyc
			  ;

objeto : NOMBRE1 adjetivo CADENA resto_objeto
	   | NOMBRE_VERBO1 adjetivo CADENA resto_objeto
	   ;

resto_objeto : PESO NUM inicial tipo_objeto
			   { if (!ExisteLocalidad($<valor>3))
					 Error("Localidad inexistente");

				 if ($<valor>3 == ENCIMA && $4 != T_ROPA)
					 Error("No puede llevarse encima un objeto no-ropa");

				 if (!InsertarObjeto($4, $2, CodigoVocab($<lex>-2),
					 $<valor>-1, $<lex>0, $<valor>3, $<valor>3, -1, -1))
					 Error("No hay memoria u objeto ya existente");
			   }
			 | INICIAL loc_inic peso tipo_objeto
			   { if (!ExisteLocalidad($<valor>2))
					 Error("Localidad inexistente");

				 if ($<valor>2 == ENCIMA && $4 != T_ROPA)
					 Error("No puede llevarse encima un objeto no-ropa");

				 if (!InsertarObjeto($4, $<valor>3, CodigoVocab($<lex>-2),
					 $<valor>-1, $<lex>0, $<valor>2, $<valor>2, -1, -1))
					 Error("No hay memoria u objeto ya existente");
			   }

			 | tipo_objeto
			   { if (!InsertarObjeto($1, 1, CodigoVocab($<lex>-2),
					 $<valor>-1, $<lex>0, NO_CREADO, NO_CREADO, -1, -1))
					 Error("No hay memoria u objeto ya existente");
			   }
			 ;

peso : PESO NUM { $<valor>$ = $2; }
	 | { $<valor>$ = 1; }
	 ;

inicial : INICIAL loc_inic { $<valor>$ = $<valor>2; }
		| { $<valor>$ = NO_CREADO; }
		;

loc_inic : NO_CREADO1 { $<valor>$ = NO_CREADO; }
		 | ENCIMA1    { $<valor>$ = ENCIMA; }
		 | LLEVADO2   { $<valor>$ = LLEVADO; }
		 | NUM        { $<valor>$ = $1; }
		 ;

adjetivo : ADJETIVO1 { $$ = CodigoVocab($1); }
		 |           { $$ = -1; }
		 ;

tipo_objeto : CONTENEDOR { $$ = T_CONTENEDOR; }
			| ROPA       { $$ = T_ROPA; }
			|            { $$ = T_NORMAL; }
			;

mens_sistema : SISTEMA '{' lista_sistema lla_ce
			 | error '{' lista_sistema lla_ce
			 ;

lista_sistema : lista_sistema sistema
			  | sistema
			  ;

sistema : NUM ':' CADENA pyc
		  { if ($1 >= 0 && $1 < NUM_SISTEMA) {
				if (!CambiarSistema($1, $3))
					Error("No hay memoria o mensaje del sistema ya "
						  "existente");
			} else
				Error("Mensaje de sistema inexistente");
		  }
		;

respuesta : verbo nombre
			{ if (InsertarRespuesta($1, $2, despl) == 0)
				  Error("No hay memoria o respuesta ya existe");
			}
			'{' lista_sentencias lla_ce
			{ AsignaCod(RETSBR, 1); }
		  | verbo error '{' lista_sentencias lla_ce
		  | error nombre '{' lista_sentencias lla_ce
		  ;

verbo : VERBO1  { $$ = CodigoVocab($1); }
	  | NOMBRE_VERBO1 { $$ = CodigoVocab($1); }
	  | DIRECCION1 { $$ = CodigoVocab($1); }
	  | '*'     { $$ = -1; }
	  | '_'     { $$ = -2; }
	  ;

nombre : NOMBRE1    { $$ = CodigoVocab($1); }
	   | NOMBRE_VERBO1 { $$ = CodigoVocab($1); }
	   | DIRECCION1 { $$ = CodigoVocab($1); }
	   | '*'        { $$ = -1; }
	   | '_'        { $$ = -2; }
	   ;

lista_sentencias : lista_sentencias sentencia
				 | sentencia
				 ;

sentencia : compuesta
		  | while_do
		  | if_then
		  | if_then_else
		  | evaluar
		  | simple pyc
		  | simple error { Error("Falta el ';' terminador de sentencia"); }
		  | error pyc
		  ;

simple : asignacion
	   | llamada_subrutina
	   | retorno
	   | coger
	   | dejar
	   | poner
	   | quitar
	   | crear
	   | destruir
	   | acabar
	   | turnos
	   | hecho
	   | no_hecho
	   | colocar
	   | intercambiar
	   | meter
	   | sacar
	   | autoc
	   | autod
	   | autop
	   | autoq
	   | autom
	   | autos
	   | listaobj
	   | listade
	   | listacont
	   | inven
	   | mensaje
	   | mensis
	   | tecla
	   | pausa
	   | describe
	   | mueve
	   | diag
	   | imprime
	   | salvar
	   | cargar
	   | borrar
	   | salidas
	   | cursor
	   | saltolinea
	   | imagen
	   | sonido
	   | silencio
	   | ventana
	   | color
	   | fondo
	   | fuente
	   ;

compuesta : '{' lista_sentencias lla_ce
		  ;

asignacion : bandera ASIGNA rvalue
			 { AsignaCod(ALMACENA, 1); }
		   | bandera EQU rvalue
			 { Error("El operador de asignaci¢n debe ser \":=\" y no "
					 "\"=\""); }
		   | bandera error rvalue
		   ;

bandera : BANDERA { AsignaCod(EMPUJA, 0); AsignaArg($1, 1); }
		;

llamada_subrutina : LLAMA '(' ID par_ce
					{   int n; SUBR *f;
						if ((f = DarSubrutina($3)) == NULL) {
							n = gen_etiq();
							if (InsertarSubrutina($3, -n) == 0)
								Error("No hay memoria");
						} else
							n = abs(DarEtiquetaSubrutina(f));

						AsignaCod(SALTASBR, 0);
						AsignaSig(S_ETIQUETA, 0);
						AsignaArg(n, 1);
					}
				  ;

rvalue : rvalue '+' rvalue { AsignaCod('+', 1); }
	   | rvalue '-' rvalue { AsignaCod('-', 1); }
	   | rvalue '*' rvalue { AsignaCod('*', 1); }
	   | rvalue '/' rvalue { AsignaCod('/', 1); }
	   | rvalue '%' rvalue { AsignaCod(C_MODULO, 1); }
	   | '(' rvalue par_ce
	   | '-' rvalue %prec MENOSU { AsignaCod(C_MENOSU, 1); }
	   | '+' rvalue %prec MENOSU
	   | lvalue  { AsignaCod(CARGA, 1); }
	   | cadena
	   | NUM     { AsignaCod(EMPUJA, 0);
				   AsignaArg($1, 1); }
	   | NOMBRE1 adjetivo
		 { int n = CodigoObjeto(CodigoVocab($1), $2);
		   if (n == -1)
			   Error("El objeto no existe");
		   else {
			   AsignaCod(EMPUJA, 0);
			   AsignaArg(n, 1);
		   }
		 }
	   | OBJETO '(' rvalue ',' rvalue par_ce
		 { AsignaCod(C_OBJETO, 1); }
	   | OBJETO '(' par_ce
		 { AsignaCod(C_OBJETO_DEFECTO, 1); }
	   | NOT rvalue                { AsignaCod(C_NOT, 1); }
	   | rvalue AND rvalue         { AsignaCod(C_AND, 1); }
	   | rvalue OR  rvalue         { AsignaCod(C_OR, 1); }
	   | rvalue MAY rvalue         { AsignaCod(C_MAY, 1); }
	   | rvalue MEN rvalue         { AsignaCod(C_MEN, 1); }
	   | rvalue MAI rvalue         { AsignaCod(C_MAI, 1); }
	   | rvalue MEI rvalue         { AsignaCod(C_MEI, 1); }
	   | rvalue EQU rvalue         { AsignaCod(C_EQU, 1); }
	   | rvalue NEQ rvalue         { AsignaCod(C_NEQ, 1); }
	   | NO_CREADO1                { AsignaCod(EMPUJA, 0);
									 AsignaArg(NO_CREADO, 1); }
	   | ENCIMA1                   { AsignaCod(EMPUJA, 0);
									 AsignaArg(ENCIMA, 1); }
	   | LLEVADO2                  { AsignaCod(EMPUJA, 0);
									 AsignaArg(LLEVADO, 1); }
	   | ESTA '(' rvalue par_ce       { AsignaCod(C_ESTA, 1); }
	   | ESTAMAY '(' rvalue par_ce    { AsignaCod(C_ESTAMAY, 1); }
	   | ESTAMEN '(' rvalue par_ce    { AsignaCod(C_ESTAMEN, 1); }
	   | PRESENTE '(' rvalue par_ce   { AsignaCod(C_PRESENTE, 1); }
	   | AUSENTE '(' rvalue par_ce    { AsignaCod(C_AUSENTE, 1); }
	   | PUESTO1 '(' rvalue par_ce    { AsignaCod(C_PUESTO, 1); }
	   | LLEVADO1 '(' rvalue par_ce   { AsignaCod(C_LLEVADO, 1); }
	   | ESTAEN '(' rvalue ',' rvalue par_ce
		 { AsignaCod(C_ESTAEN, 1); }
	   | LOCAL '(' rvalue par_ce      { AsignaCod(C_LOCAL, 1); }
	   | VERB1 '(' verbo1 par_ce
		 { AsignaCod(EMPUJA, 0);
		   AsignaArg(CodigoVocab($3), 1);
		   AsignaCod(C_VERB1, 1);
		 }
	   | NOMB1 '(' nombre_verbo par_ce
		 { AsignaCod(EMPUJA, 0);
		   AsignaArg(CodigoVocab($3), 1);
		   AsignaCod(C_NOMB1, 1);
		 }
	   | ADJET1 '(' ADJETIVO1 par_ce
		 { AsignaCod(EMPUJA, 0);
		   AsignaArg(CodigoVocab($3), 1);
		   AsignaCod(C_ADJET1, 1);
		 }
	   | ADVERB '(' ADVERBIO1 par_ce
		 { AsignaCod(EMPUJA, 0);
		   AsignaArg(CodigoVocab($3), 1);
		   AsignaCod(C_ADVERB, 1);
		 }
	   | PREPOS '(' PREPOSICION1 par_ce
		 { AsignaCod(EMPUJA, 0);
		   AsignaArg(CodigoVocab($3), 1);
		   AsignaCod(C_PREPOS, 1);
		 }
	   | NOMB2 '(' NOMBRE1 par_ce
		 { AsignaCod(EMPUJA, 0);
		   AsignaArg(CodigoVocab($3), 1);
		   AsignaCod(C_NOMB2, 1);
		 }
	   | ADJET2 '(' ADJETIVO1 par_ce
		 { AsignaCod(EMPUJA, 0);
		   AsignaArg(CodigoVocab($3), 1);
		   AsignaCod(C_ADJET2, 1);
		 }
	   | VERBO1
		 { AsignaCod(EMPUJA, 0);
		   AsignaArg(CodigoVocab($1), 1);
		 }
	   | NOMBRE_VERBO1
		 { AsignaCod(EMPUJA, 0);
		   AsignaArg(CodigoVocab($1), 1);
		 }
	   | DIRECCION1
		 { AsignaCod(EMPUJA, 0);
		   AsignaArg(CodigoVocab($1), 1);
		 }
	   | ACABAR1 { AsignaCod(C_ACABAR, 1); }
	   | ANALIZA { AsignaCod(C_ANALIZA, 1); }
	   | ALEATORIO '(' rvalue par_ce { AsignaCod(C_ALEATORIO, 1); }
	   | PRIMERO '(' rvalue par_ce
		 { AsignaCod(C_PRIMERO, 1); }
	   | SIGUIENTE '(' rvalue par_ce
		 { AsignaCod(C_SIGUIENTE, 1); }
	   | PESO '(' rvalue par_ce
		 { AsignaCod(C_PESO, 1); }
	   | CONTENEDOR1 '(' rvalue par_ce
		 { AsignaCod(C_CONTENEDOR, 1); }
	   | DIRECC1 '(' rvalue par_ce
		 { AsignaCod(C_DIRECC1, 1);
		 }
	   | ROPA1 '(' rvalue par_ce
		 { AsignaCod(C_ROPA, 1); }
	   | POSX '(' par_ce
		 { AsignaCod(C_POSX, 1); }
	   | POSY '(' par_ce
		 { AsignaCod(C_POSY, 1); }
	   | CAMINAR '(' rvalue ',' rvalue par_ce
		 { AsignaCod(C_CAMINAR, 1); }
	   | '(' error par_ce
	   | '(' rvalue error
	   | rvalue error rvalue
	   | error
	   ;

verbo1 : VERBO1
	   | NOMBRE_VERBO1
	   | DIRECCION1
	   ;

nombre_verbo : NOMBRE1
			 | NOMBRE_VERBO1
			 | DIRECCION1
			 ;

lvalue : BANDERA { AsignaCod(EMPUJA, 0);
				   AsignaArg($1, 1); }
	   ;

retorno : RETORNO { AsignaCod(RETSBR, 1); }
		;

while_do : MIENTRAS marca_while1 '(' rvalue par_ce
		   marca_while2
		   sentencia
		   { AsignaCod(SALTA, 0);
			 AsignaArg($<valor>2, 1);
			 if ($<linea>6 >= TAM_CODIGO)
				 Error("No hay memoria para generar mas c¢digo");
			 else
				 codigo[$<linea>6].argumento = despl;
		   }
		 | MIENTRAS marca_while1 '(' rvalue error marca_while2 sentencia
		 ;

/* Se usan dos marcadores porque as¡ podemos poner la segunda regla
   del mientras para la recuperaci¢n de errores. Si no las ponemos, nos
   sale un conflicto reduce/reduce */

marca_while1 : { $<linea>$ = despl; }
			 ;

marca_while2 : { $<linea>$ = despl; AsignaCod(SALTAFALSO, 1); }
			 ;

if_then : SI '(' rvalue par_ce marcador sentencia
		  { if ($<linea>5 >= TAM_CODIGO)
				Error("No hay memoria para generar mas c¢digo");
			else
				codigo[$<linea>5].argumento = despl; }
		| SI '(' rvalue error marcador sentencia
		;

if_then_else : SI '(' rvalue par_ce marcador sentencia SINO
			   { $<linea>$ = despl;
				 AsignaCod(SALTA, 1);
				 if ($<linea>5 >= TAM_CODIGO)
					 Error("No hay memoria para generar mas c¢digo");
				 else
					 codigo[$<linea>5].argumento = despl;
			   }
			   sentencia
			   { if ($<linea>8 >= TAM_CODIGO)
					 Error("No hay memoria para generar mas c¢digo");
				 else
					 codigo[$<linea>8].argumento = despl; }
			 | SI '(' rvalue error marcador sentencia SINO sentencia
			 ;

marcador : { $<linea>$ = despl; AsignaCod(SALTAFALSO, 1); }
		 ;

evaluar : EVALUAR '(' marca_eval1
		  rvalue par_ce
		  { AsignaCod(RETSBR, 1);
			if ($<linea>3 >= TAM_CODIGO)
				Error("No hay memoria para generar mas c¢digo");
			else
				codigo[$<linea>3].argumento = despl;
		  }
		  '{' casos defecto lla_ce
		  { AsignaEti(siguiente, 0);
			InsertarEtiqueta(siguiente, despl);
		  }
		| EVALUAR '(' marca_eval1 rvalue error '{' casos defecto lla_ce
		;

/* Al igual que ocurre con el mientras, usamos un marcador para hacer
   posible meter la segunda regla y hacer recuperaci¢n de errores */

marca_eval1 : { unsigned long n = despl; siguiente = gen_etiq();
				AsignaCod(SALTA, 1);
				eval = despl; $<linea>$ = n;
			  }
			;

casos : casos caso
	  |
	  ;

caso : CASO rvalue ':'
	   { unsigned long n;
		 AsignaCod(SALTASBR, 0);
		 AsignaArg(eval, 1);
		 AsignaCod(C_EQU, 1);
		 n = despl;
		 AsignaCod(SALTAFALSO, 1);
		 $<linea>$ = n;
	   }
	   sentencia
	   { AsignaCod(SALTA, 0);
		 AsignaArg(siguiente, 0);
		 AsignaSig(S_ETIQUETA, 1);
		 if ($<linea>4 >= TAM_CODIGO)
			 Error("No hay memoria para generar mas c¢digo");
		 else
			 codigo[$<linea>4].argumento = despl;
	   }
	 ;

defecto : DEFECTO ':' sentencia
		|
		;

coger : COGER '(' rvalue par_ce { AsignaCod(A_COGER, 1); }
	  ;

dejar : DEJAR '(' rvalue par_ce { AsignaCod(A_DEJAR, 1); }
	  ;

poner : PONER '(' rvalue par_ce { AsignaCod(A_PONER, 1); }
	  ;

quitar : QUITAR '(' rvalue par_ce { AsignaCod(A_QUITAR, 1); }
	   ;

crear : CREAR '(' rvalue par_ce { AsignaCod(A_CREAR, 1); }
	  ;

destruir : DESTRUIR '(' rvalue par_ce { AsignaCod(A_DESTRUIR, 1); }
		 ;

acabar : ACABAR { AsignaCod(A_ACABAR, 1); }
	   ;

turnos : TURNOS { AsignaCod(A_TURNOS, 1); }
	   ;

hecho : HECHO { AsignaCod(A_HECHO, 1); }
	  ;

no_hecho : NO_HECHO { AsignaCod(A_NO_HECHO, 1); }
		 ;

colocar : COLOCAR '(' rvalue ',' rvalue par_ce
		  { AsignaCod(A_COLOCAR, 1); }
		| COLOCAR '(' rvalue error rvalue par_ce
		;

intercambiar : INTERCAMBIAR '(' rvalue ',' rvalue par_ce
			   { AsignaCod(A_INTERCAMBIAR, 1); }
			 | INTERCAMBIAR '(' rvalue error rvalue par_ce
			 ;

meter : METER '(' rvalue ',' rvalue par_ce { AsignaCod(A_METER, 1); }
	  | METER '(' rvalue error rvalue par_ce
	  ;

sacar : SACAR '(' rvalue ',' rvalue par_ce { AsignaCod(A_SACAR, 1); }
	  | SACAR '(' rvalue error rvalue par_ce
	  ;

autoc : AUTOC { AsignaCod(A_AUTOC, 1); }
	  ;

autod : AUTOD { AsignaCod(A_AUTOD, 1); }
	  ;

autop : AUTOP { AsignaCod(A_AUTOP, 1); }
	  ;

autoq : AUTOQ { AsignaCod(A_AUTOQ, 1); }
	  ;

autom : AUTOM '(' rvalue par_ce { AsignaCod(A_AUTOM, 1); }
	  ;

autos : AUTOS '(' rvalue par_ce { AsignaCod(A_AUTOS, 1); }
	  ;

listaobj : LISTAOBJ { AsignaCod(A_LISTAOBJ, 1); }
		 ;

listade : LISTADE '(' rvalue par_ce { AsignaCod(A_LISTADE, 1); }
		;

listacont : LISTACONT '(' rvalue par_ce { AsignaCod(A_LISTACONT, 1); }
		  ;

inven : INVEN { AsignaCod(A_INVEN, 1); }
	  ;

mensaje : cadena { AsignaCod(A_MENS, 1); }
		;

cadena : CADENA
		 { int n = BuscarMensaje($1);

		   if (n != -1) {
			   AsignaCod(EMPUJA, 0);
			   AsignaArg(n, 1);
		   } else {
			   n = gen_mens();
			   if (!InsertarMensaje(n, $1))
				   Error("No hay memoria para una cadena mas");
			   AsignaCod(EMPUJA, 0);
			   AsignaArg(n, 1);
		   }

		 }
	   ;

mensis : MENSIS '(' rvalue par_ce { AsignaCod(A_MENSIS, 1); }
	   ;

imprime : IMPRIME '(' rvalue par_ce { AsignaCod(A_IMPRIME, 1); }
		;

diag : DIAG { AsignaCod(A_DIAG, 1); }
	 ;

tecla : TECLA { AsignaCod(A_TECLA, 1); }
	  ;

pausa : PAUSA '(' rvalue par_ce { AsignaCod(A_PAUSA, 1); }
	  ;

describe : DESCRIBE { AsignaCod(A_DESCRIBE, 1); }
		 ;

mueve : MUEVE '(' rvalue par_ce { AsignaCod(A_MUEVE, 1); }
	  ;

salvar : SALVAR { AsignaCod(A_SALVAR, 1); }
	   ;

cargar : CARGAR { AsignaCod(A_CARGAR, 1); }
	   ;

borrar : BORRAR { AsignaCod(A_BORRAR, 1); }
	   ;

salidas : SALIDAS { AsignaCod(A_SALIDAS, 1); }
		;

cursor : CURSOR '(' rvalue ',' rvalue par_ce
		 { AsignaCod(A_CURSOR, 1); }
	   | CURSOR '(' rvalue error rvalue par_ce
	   ;

saltolinea : SALTOLINEA { AsignaCod(A_SALTOLINEA, 1); }
		   ;

imagen : IMAGEN '(' cadena par_ce
		 { AsignaCod(A_IMAGEN, 1); }
	   | IMAGEN_CICLO '(' cadena par_ce
		 { AsignaCod(A_IMAGEN2, 1); }
	   ;

sonido : SONIDO '(' cadena par_ce
		 { AsignaCod(A_SONIDO, 1); }
	   | SONIDO_CICLO '(' cadena par_ce
		 { AsignaCod(A_SONIDO2, 1); }
	   ;

silencio : SILENCIO '(' cadena par_ce
		   { AsignaCod(A_SILENCIO, 1); }
		 ;

ventana : VENTANA '(' rvalue ',' rvalue par_ce
		  { AsignaCod(A_VENTANA, 1); }
		| VENTANA '(' rvalue error rvalue par_ce
		;

color : COLOR '(' rvalue par_ce { AsignaCod(A_COLOR, 1); }
	  ;

fondo : FONDO '(' rvalue par_ce { AsignaCod(A_FONDO, 1); }
	  ;

fuente : FUENTE '(' cadena par_ce { AsignaCod(A_FUENTE, 1); }
	   ;

subrutina : SUBRUTINA ID
			{ int n; SUBR *f;
				if ((f = DarSubrutina($2)) == NULL) {
					n = gen_etiq();
					if (InsertarSubrutina($2, n) == 0)
						Error("No hay memoria para insertar subrutina");
				} else {
					n = DarEtiquetaSubrutina(f);
					if (n >= 0)
						Error("Redeclaraci¢n de subrutina");
					else
						FijarEtiquetaSubrutina(f, abs(n));
				}

				InsertarEtiqueta(abs(n), despl);
				AsignaEti(abs(n), 0);
			}
			'{' lista_sentencias lla_ce
			{ AsignaCod(RETSBR, 1); }
		  | SUBRUTINA error '{' lista_sentencias lla_ce
		  | error ID '{' lista_sentencias lla_ce
		  ;

par_ce : ')' { yyerrok; }
	   ;

lla_ce : '}' { yyerrok; }
	   ;

pyc : ';' { yyerrok; }
	;

%%

void yyerror(int err, int tok)
{
	static int aux = 0;

	if (err == -1)
		if (tok != -1) {
			if (aux == 0) {
				numErrores++;
				printf("(%d) ERROR EN LINEA %u:\n", numErrores, numLinea);
				printf("Se encontr¢: %s\n", yydisplay(token, 1));
				printf("y se esperaba: ");
				aux = 1;
			} else {
				if (strlen(yydisplay(tok, 0)) + wherex() >= 80)
					printf("\n%s  ", yydisplay(tok, 0));
				else
					printf("%s  ", yydisplay(tok, 0));
			}
		} else {
			printf("\nPulse una tecla seguir...");
			getch();
			printf("\n\n");
			aux = 0;
		}

}


int gen_etiq(void)
{
	static int e = 1;
	if (e >= 32767)
		Error("No se pueden asignar m s etiquetas internas");
	return e++;
}


int gen_mens(void)
{
	static int m = 0;
	if (m >= 32767)
		Error("No se pueden reservar m s cadenas");
	return m++;
}

void AsignaEti(int etiq, int inc)
{
	codigo[despl].etiqueta = etiq;
	despl = (inc == 1) ? despl + 1 : despl;
	if (despl >= TAM_CODIGO)
		Error("No hay memoria para generar m s c¢digo");
}

void AsignaCod(char cod, int inc)
{
	codigo[despl].codop = cod;
	despl = (inc == 1) ? despl + 1 : despl;
	if (despl >= TAM_CODIGO)
		Error("No hay memoria para generar m s c¢digo");
}

void AsignaArg(int arg, int inc)
{
	codigo[despl].argumento = arg;
	despl = (inc == 1) ? despl + 1 : despl;
	if (despl >= TAM_CODIGO)
		Error("No hay memoria para generar m s c¢digo");
}

void AsignaSig(char sig, int inc)
{
	codigo[despl].signif = sig;
	despl = (inc == 1) ? despl + 1 : despl;
	if (despl >= TAM_CODIGO)
		Error("No hay memoria para generar m s c¢digo");
}
