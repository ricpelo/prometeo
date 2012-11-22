#include <dos.h>
#include <ctype.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <alloc.h>
#include <dir.h>
#include "T_Mens.H"
#include "Sistema.H"
#include "Lexico.H"
#include "Vocab.H"
#include "T_Objeto.H"
#include "Etiq.H"
#include "T_Resp.H"
#include "Codigo.H"
#include "Local.H"
#include "T_Subr.H"
#include "yytab.h"
#include "Memoria.H"
#include "Bandera.H"

#define ungetch(c)			ungetc(c, fp)

int yyparse(void);
void SegundoPaso(void);
void CompilacionSeparada(FILE *lista);
void CompilarModulo(void);
void Error(char *s);
void LimpiarMemoria(void);

typedef struct {				/* La tabla de palabras reservadas tiene */
	char *lex;					/* cada elemento del tipo ELEM: el lexema */
	int token;					/* de la palabra y su token correspondiente */
} ELEM;

char fich[MAXPATH],			/* Cadenas usadas para contener el nombre */
	 drive[MAXDRIVE],		/* completo, unidad, directorio, fichero */
	 dir[MAXDIR],			/* y extensi¢n del fichero fuente. Son */
	 file[MAXFILE],			/* usadas por fnsplit() y fnmerge() */
	 ext[MAXEXT];

unsigned long TAM_CODIGO = 10240UL;
char yytext[LONG_BUFFER];		/* El b£ffer de entrada */
unsigned int numLinea = 1;	    /* N£mero de l¡nea actual en fich. */
int token;						/* Token actual de la entrada */
int numErrores;					/* N£mero de errores sint cticos */
FILE *fp;						/* Fichero de entrada */
CODIGO *codigo;					/* El c¢digo se genera en este vector */
unsigned long despl;			/* Desplazamiento actual dentro del vector
								   de c¢digo */


/*
	main():	Programa principal
	Recibe: Toma como argumento el nombre del fichero donde est  el
			c¢digo fuente
*/
int main(int argc, char *argv[])
{
	CODIGO c;
	struct ffblk fb;
	int i, flag;
	FILE *lista;
	int swErr = 0;
/*	int salida = 0;
	char far *p;

	p = (char *) getvect(0xF5);
	p += 0x145C;

	if (*p   == '<')
	if (*(p+1) == ':')
	if (*(p+2) == 'G')
	if (*(p+3) == 'A')
	if (*(p+4) == 'C')
	if (*(p+5) == ':')
	if (*(p+6) == '>')
		salida = 1;

	if (!salida) {
		printf("\nEl programa LLAVE no est  residente en memoria\n\n");
		exit(1);
	}
*/
	printf("PROMETEO 2.2 - Generador de Aventuras Conversacionales\n");
	printf("Compilador (c) 1995-97 Ricardo P‚rez L¢pez - Alpha Aventuras\n");
	LimpiarMemoria();

	if (argc == 1)

		if (findfirst("*.PRO", &fb, 0) == -1) {
			printf("\n\tSintaxis: PC [ fichero[.PRO] | @fichero[.PL] ] "
				   "[-mc<tama¤o_codigo>]\n");
			exit(1);
		} else {
			printf("(usar opci¢n /h o /? para instrucciones de uso)\n\n");
			printf("Encontrado fichero %s. Se procede a compilarlo.\n",
				   fb.ff_name);
			getch();
			strcpy(fich, fb.ff_name);
			CompilarModulo();
		}

	else {

		if (argv[1][0] == '@') {
			strcpy(fich, (argv[1] + 1));
			flag = fnsplit(fich, drive, dir, file, ext);

			if (!(flag & EXTENSION)) {
				strcpy(ext, ".PL");
				fnmerge(fich, drive, dir, file, ext);
			}

			if ((lista = fopen(fich, "rt")) == NULL) {
				printf("El fichero \"%s\" no existe\n", fich);
				exit(1);
			} else {
				printf("(usar opci¢n /h o /? para instrucciones de uso)\n\n");
				CompilacionSeparada(lista);
			}

		} else if (argv[1][0] == '/' || argv[1][0] == '-') {

			switch (argv[1][1]) {
				case 'h':
				case 'H':
				case '?':
					printf("\n\tSintaxis: PC [ fichero[.PRO] | "
							"@fichero[.PL] ] [-mc<tama¤o_codigo>]\n");
					exit(1);
					break;

				default:
					printf("\n\tOpci¢n desconocida\n");
					exit(1);

			}

		} else {
			printf("(usar opci¢n /h o /? para instrucciones de uso)\n\n");

			if (strncmpi(argv[2], "-mc", 3) == 0)
				TAM_CODIGO = atol(argv[2] + 3);

			strcpy(fich, argv[1]);
			CompilarModulo();
		}

	}

	if (numErrores == 0) {
		printf("\nSegundo paso...\n");
		SegundoPaso();
		flag = fnsplit(fich, drive, dir, file, ext);
		strcpy(ext, ".GAM");
		fnmerge(fich, drive, dir, file, ext);
		printf("Generando c¢digo en \"%s\"...\n", fich);
		fp = fopen(fich, "w+b");
		fprintf(fp, "PRO");
		if (numPal == 0) {
			printf("ERROR: No se han definido palabras");
			swErr = 1;
		}
		if (numResp == 0) {
			printf("ERROR: No se han definido respuestas");
			swErr = 1;
		}
		if (numLocal == 0) {
			printf("ERROR: No se han definido localidades");
			swErr = 1;
		}
		if (!ExisteLocalidad(0)) {
			printf("ERROR: La localidad 0 no existe");
			swErr = 1;
		}
		if (swErr)
			exit(1);
		SalvarVocab(fp);
		SalvarLocalidad(fp);
		SalvarObjeto(fp);
		SalvarMensaje(fp);
		SalvarSistema(fp);
		SalvarRespuesta(fp);
		SalvarSubrutina(fp);

		despl++;
		fwrite(&despl, sizeof despl, 1, fp);	/* Generamos el c¢digo */

		for (i = 0; i < despl; i++) {
			c = codigo[i];
			fwrite(&c.codop, sizeof c.codop, 1, fp);
			fwrite(&c.argumento, sizeof c.argumento, 1, fp);
		}

		fclose(fp);
		printf("C¢digo generado\n\n");
		printf("C¢digo interno: %lu instrucciones\n", despl);
		printf("Palabras en el vocabulario: %d\n", numPal);
		printf("Localidades: %d\n", numLocal);
		printf("Objetos: %d\n", numObj);
		printf("Cadenas de caracteres: %d\n", numMens);
		printf("Entradas en tabla de respuestas: %d\n", numResp);
	} else if (numErrores == 1)
		printf("\n-------------------\nSe ha cometido 1 error\n");
	else
		printf("\n-------------------\nSe han cometido %d errores\n",
			   numErrores);


	return 0;
}


/*
	SegundoPaso():	Hace una pasada completa por el vector de c¢digo, para
					sustituir las referencias a etiquetas por direcciones
					dentro del vector
*/
void SegundoPaso(void)
{
	int i;
	int salida = 0;
	SUBR *s;

	for (i = 0; i < TABLA_SUBRUTINA; i++) {
		s = tablaSubr[i];
		while (s != NULL) {
			if (s->etiqueta < 0) {
				salida = 1;
				printf("ERROR: La subrutina %s no ha sido declarada\n",
						s->nombre);
			}
			s = s->siguiente;
		}
	}

	if (salida)
		exit(1);

	for (i = 0; i < despl; i++) {
		switch(codigo[i].codop) {
			case SALTASBR:
			case SALTAFALSO:
			case SALTA:
				if (codigo[i].signif == S_ETIQUETA) {
					codigo[i].argumento =
						DireccionEtiqueta(codigo[i].argumento);
					codigo[i].signif = S_DIRECCION;
				}
		}
	}
}


void CompilacionSeparada(FILE *lista)
{
	char *p;

	p = fgets(fich, MAXPATH + 1, lista);

	if (p == NULL) {
		printf("No hay m¢dulos que compilar\n");
		exit(1);
	}

	while (p != NULL) {
		*(strchr(fich, '\n')) = '\0';
		if (fich[0] != '\0')
			CompilarModulo();
		p = fgets(fich, MAXPATH + 1, lista);
	}

}


void CompilarModulo(void)
{
	int flag;

	flag = fnsplit(fich, drive, dir, file, ext);

	if (!(flag & EXTENSION)) {
		strcpy(ext, ".PRO");
		fnmerge(fich, drive, dir, file, ext);
	}

	if ((fp = fopen(fich, "rt")) == NULL) {
		printf("El fichero \"%s\" no existe\n", fich);
		exit(1);
	}

	if (codigo == NULL) {
		codigo = (CODIGO *) Memoria(sizeof(CODIGO) * TAM_CODIGO);
		memset(codigo, 0, sizeof(CODIGO) * TAM_CODIGO);
	}

	printf("Compilando \"%s\":\n", fich);
	printf("Primer paso...\n");
	yyparse();
	fclose(fp);
}



void LimpiarMemoria(void)
{
	int i;

	memset(tablaPal, NULL, sizeof tablaPal);
	memset(indicePal, NULL, sizeof indicePal);
	memset(tablaMens, NULL, sizeof tablaMens);
	memset(sistema, NULL, sizeof sistema);
	memset(tablaObj, NULL, sizeof tablaObj);
	memset(indiceObj, NULL, sizeof indiceObj);

	for (i = 0; i < NUM_LOCALIDADES; i++) {
		local[i].descripcion = NULL;
		local[i].pantalla = -1;
		local[i].conexion = NULL;
	}

	memset(etiquetas, 0, sizeof etiquetas);
	memset(tablaResp, NULL, sizeof tablaResp);
	memset(tablaSubr, NULL, sizeof tablaSubr);
	despl = 0;
	numErrores = 0;
	codigo = NULL;
}


/*
	yylex(): 	El analizador l‚xico del compilador
	Devuelve:	El token encontrado en la entrada
	Notas:		La funci¢n es llamada autom ticamente por el analizador
				sint ctico yyparse()
*/
int yylex(void)
{
	static ELEM tabla[] = { { "acabar", ACABAR },
							{ "acabar?", ACABAR1 },
							{ "adjetivo", ADJETIVO },
							{ "adjetivo1?", ADJET1 },
							{ "adjetivo2?", ADJET2 },
							{ "adverbio", ADVERBIO },
							{ "adverbio?", ADVERB },
							{ "aleatorio?", ALEATORIO },
							{ "analiza?", ANALIZA },
							{ "ausente?", AUSENTE },
							{ "autoc", AUTOC },
							{ "autod", AUTOD },
							{ "autom", AUTOM },
							{ "autop", AUTOP },
							{ "autoq", AUTOQ },
							{ "autos", AUTOS },
							{ "borrar", BORRAR },
							{ "caminar", CAMINAR },
							{ "cargar", CARGAR },
							{ "caso", CASO },
							{ "coger", COGER },
							{ "colocar", COLOCAR },
							{ "color", COLOR },
							{ "conjuncion", CONJUNCION },
							{ "contenedor", CONTENEDOR },
							{ "contenedor?", CONTENEDOR1 },
							{ "crear", CREAR },
							{ "cursor", CURSOR },
							{ "defecto", DEFECTO },
							{ "dejar", DEJAR },
							{ "desc", DESC },
							{ "describe", DESCRIBE },
							{ "destruir", DESTRUIR },
							{ "diag", DIAG },
							{ "direccion", DIRECCION },
							{ "direccion?", DIRECC1 },
							{ "encima", ENCIMA1 },
							{ "esta?", ESTA },
							{ "estaen?", ESTAEN },
							{ "estamay?", ESTAMAY },
							{ "estamen?", ESTAMEN },
							{ "evaluar", EVALUAR },
							{ "false", FALSE },
							{ "fondo", FONDO },
							{ "fuente", FUENTE },
							{ "hecho", HECHO },
							{ "imagen", IMAGEN },
							{ "imagen_ciclo", IMAGEN_CICLO },
							{ "imprime", IMPRIME },
							{ "inicial", INICIAL },
							{ "intercambiar", INTERCAMBIAR },
							{ "inven", INVEN },
							{ "listacont", LISTACONT },
							{ "listade", LISTADE },
							{ "listaobj", LISTAOBJ },
							{ "llama", LLAMA },
							{ "llevado", LLEVADO2 },
							{ "llevado?", LLEVADO1 },
							{ "local", LOCAL },
							{ "localidades", LOCALIDADES },
							{ "mensajes", MENSAJES },
							{ "mensis", MENSIS },
							{ "meter", METER },
							{ "mientras", MIENTRAS },
							{ "mueve", MUEVE },
							{ "no", NOT },
							{ "no_creado", NO_CREADO1 },
							{ "no_hecho", NO_HECHO },
							{ "nombre", NOMBRE },
							{ "nombre1?", NOMB1 },
							{ "nombre2?", NOMB2 },
							{ "nombre_verbo", NOMBRE_VERBO },
							{ "o", OR },
							{ "objeto", OBJETO },
							{ "objetos", OBJETOS },
							{ "pausa", PAUSA },
							{ "peso", PESO },
							{ "poner", PONER },
							{ "posx", POSX },
							{ "posy", POSY },
							{ "preposicion", PREPOSICION },
							{ "preposicion?", PREPOS },
							{ "presente?", PRESENTE },
							{ "primero", PRIMERO },
							{ "pronombre", PRONOMBRE },
							{ "puesto?", PUESTO1 },
							{ "quitar", QUITAR },
							{ "retorno", RETORNO },
							{ "ropa", ROPA },
							{ "ropa?", ROPA1 },
							{ "sacar", SACAR },
							{ "salidas", SALIDAS },
							{ "saltolinea", SALTOLINEA },
							{ "salvar", SALVAR },
							{ "si", SI },
							{ "siguiente", SIGUIENTE },
							{ "silencio", SILENCIO },
							{ "sino", SINO },
							{ "sistema", SISTEMA },
							{ "sonido", SONIDO },
							{ "sonido_ciclo", SONIDO_CICLO },
							{ "subrutina", SUBRUTINA },
							{ "tecla", TECLA },
							{ "true", TRUE },
							{ "turnos", TURNOS },
							{ "ventana", VENTANA },
							{ "verbo", VERBO },
							{ "verbo?", VERB1 },
							{ "vocabulario", VOCABULARIO },
							{ "y", AND },
							{ "-", 0 }
						  };
	char *p;
	PALABRA *palabra;
	int c, i, encontrado, estado = 0, pos = 0;

	memset(yytext, '\0', sizeof(yytext));	/* Limpiamos el b£ffer */
	printf("%05u", numLinea);
	gotoxy(1, wherey());

	while (1) {
		switch (estado) {
			case 0:
				c = getc(fp);
				if (isspace(c)) {
					estado = 0;
					if (c == '\n')
						numLinea++;
				} else if (isdigit(c)) {
					yytext[pos++] = c;
					estado = 1;
				} else if (c == 'b') {
					yytext[pos++] = c;
					estado = 2;
				} else if (EsAlfa(c) && c != 'b') {
					yytext[pos++] = c;
					estado = 4;
				} else if (c == '\'') {
					estado = 4;
				} else if (c == '<') {
					yytext[pos++] = c;
					estado = 5;
				} else if (c == '>') {
					yytext[pos++] = c;
					estado = 8;
				} else if (c == '=') {
					yytext[pos++] = c;
					estado = 10;
				} else if (c == '\"') {
					estado = 13;
				} else if (c == '/')
					estado = 15;
				else if (c == ':') {
					yytext[pos++] = c;
					estado = 11;
				} else {
					yytext[pos++] = c;
/*					printf("CARACTER %c\n", c); */
					token = c;
					return c;
				}
				break;

			case 1:
				c = getc(fp);
				if (isdigit(c)) {
					yytext[pos++] = c;
					estado = 1;
				} else {
					ungetch(c);
					yylval.valor = atoi(yytext);
/*					printf("NUMERO %s\n", yytext); */
					token = NUM;
					return NUM;
				}
				break;

			case 2:
				c = getc(fp);
				if (isdigit(c)) {
					yytext[pos++] = c;
					estado = 3;
				} else {
					ungetch(c);
					estado = 4;
				}
				break;

			case 3:
				c = getc(fp);
				if (isdigit(c)) {
					yytext[pos++] = c;
					estado = 3;
				} else {
					ungetch(c);
					yylval.valor = atoi(yytext + 1);
					if (yylval.valor >= 0 && yylval.valor < TAM_BANDERA - 1) {
/*						printf("BANDERA %s\n", yytext); */
						token = BANDERA;
						return BANDERA;
					} else
						Error("La bandera no existe");
				}
				break;

			case 4:
				c = getc(fp);
				if (EsAlfanum(c) || c == '?' || c == '_') {
					if (pos < LONG_BUFFER - 1) {
						yytext[pos++] = c;
						estado = 4;
					} else if (pos == LONG_BUFFER - 1) {
						printf("AVISO EN LINEA %u: Identificador demasiado "
							   "largo\n", numLinea);
						pos++;
					}
				} else {
					ungetch(c);
					p = (char*) Memoria((long) (strlen(yytext) + 1));
					strcpy(p, yytext);
					yylval.lex = p;
					palabra = DarPalabraVocab(p);
					if (palabra != NULL) {
/*						printf("ALGO GUAY %s\n", yytext); */
						switch (palabra->tipo) {
							case T_VERBO:		token = VERBO1; return VERBO1;
							case T_ADVERBIO:	token = ADVERBIO1; return ADVERBIO1;
							case T_NOMBRE:		token = NOMBRE1; return NOMBRE1;
							case T_ADJETIVO:	token = ADJETIVO1; return ADJETIVO1;
							case T_PREPOSICION:	token = PREPOSICION1; return PREPOSICION1;
							case T_CONJUNCION:	token = CONJUNCION1; return CONJUNCION1;
							case T_NOMBRE_VERBO: token = NOMBRE_VERBO1; return NOMBRE_VERBO1;
							case T_DIRECCION: token = DIRECCION1; return DIRECCION1;
						}
					} else {
						i = 0;
						while ((encontrado = strcmp(yytext, tabla[i].lex)) > 0
							   && *tabla[i].lex != '-')
							i++;
						if (encontrado == 0) {
/*							printf("PALABRA RESERVADA %s\n", yytext); */
							token = tabla[i].token;
							return tabla[i].token;
						} else {
/*							printf("IDENTIFICADOR %s\n", yytext); */
							token = ID;
							return ID;
						}
					}
				}
				break;

			case 5:
				c = getc(fp);
				if (c == '=') {
					yytext[pos++] = c;
					estado = 6;
				} else if (c == '>') {
					yytext[pos++] = c;
					estado = 7;
				} else {
					ungetch(c);
/*					printf("MEN\n"); */
					token = MEN;
					return MEN;
				}
				break;

			case 6:
/*				printf("MEI\n"); */
				token = MEI;
				return MEI;

			case 7:
/*				printf("NEQ\n"); */
				token = NEQ;
				return NEQ;

			case 8:
				c = getc(fp);
				if (c == '=') {
					yytext[pos++] = c;
					estado = 9;
				} else {
					ungetch(c);
/*					printf("MAY\n"); */
					token = MAY;
					return MAY;
				}
				break;

			case 9:
/*				printf("MAI\n"); */
				token = MAI;
				return MAI;

			case 10:
/*				printf("EQU\n"); */
				token = EQU;
				return EQU;

			case 11:
				c = getc(fp);
				if (c == '=') {
					yytext[pos++] = c;
					estado = 12;
				} else {
					yytext[pos++] = c;
					if (c == '\n')
						numLinea++;
/*					printf("CARACTER %c\n", yytext[0]); */
					token = yytext[0];
					return yytext[0];
				}
				break;

			case 12:
/*				printf("ASIGNA\n"); */
				token = ASIGNA;
				return ASIGNA;

			case 13:
				c = getc(fp);
				if (c == '\"')
					estado = 14;
				else {
					if (c == '\\') {
						c = getc(fp);
						if (c == 'n') {
							estado = 17;
							c = '\n';
						} else if (c == 't') {
							estado = 17;
							c = '\t';
						} else if (c == '\"')
							c = '\"';
						else {
							if (pos >= LONG_BUFFER - 1) {
								if (pos == LONG_BUFFER - 1) {
									printf("AVISO EN LINEA %u: Cadena "
										   "demasiado larga\n", numLinea);
									pos++;
								}
							} else
								yytext[pos++] = '\\';
						}
					} else {
						if (isspace(c)) {
							if (c == '\n')
								numLinea++;
							c = ' ';
							estado = 17;
						}
					}

					if (pos >= LONG_BUFFER - 1) {
						if (pos == LONG_BUFFER - 1) {
							printf("AVISO EN LINEA %u: Cadena demasiado "
								   "larga\n", numLinea);
							pos++;
						}
					} else
						yytext[pos++] = c;
				}

				break;

			case 14:
				p = (char*) Memoria((long) (strlen(yytext) + 1));
				strcpy(p, yytext);
				yylval.lex = p;
/*				printf("CADENA %s\n", yytext); */
				token = CADENA;
				return CADENA;

			case 15:
				c = getc(fp);
				if (c == '/')
					estado = 16;
				else {
					ungetch(c);
					yytext[0] = '/';
/*					printf("CARACTER %c\n", yytext[0]); */
					token = yytext[0];
					return yytext[0];
				}
				break;

			case 16:
				c = getc(fp);
				if (c == '\n') {
					ungetch(c);
					estado = 0;
				} else
					estado = 16;
				break;

			case 17:
				c = getc(fp);
				if (c == '\"')
					estado = 14;
				else {
					if (c == '\\') {
						c = getc(fp);
						if (c == 'n')
							c = '\n';
						else if (c == 't')
							c = '\t';
						else if (c == '\"')
							c = '\"';
						else {
							if (pos >= LONG_BUFFER - 1) {
								if (pos == LONG_BUFFER - 1) {
									printf("AVISO EN LINEA %u: Cadena "
										   "demasiado larga\n", numLinea);
									pos++;
								}
							} else
								yytext[pos++] = '\\';
						}

						if (pos >= LONG_BUFFER - 1) {
							if (pos == LONG_BUFFER - 1) {
								printf("AVISO EN LINEA %u: Cadena demasiado "
										"larga\n", numLinea);
								pos++;
							}
						} else
							yytext[pos++] = c;

						if (!isspace(c))
							estado = 13;
						else
							estado = 17;
					} else {
						if (isspace(c)) {
							if (c == '\n')
								numLinea++;
							estado = 17;
						} else {
							ungetch(c);
							estado = 13;
						}
					}
				}

				break;

		}

	}

}


void Error(char *s)
{
	printf("(%d) ERROR EN LINEA %u:\n%s", ++numErrores, numLinea, s);
	printf("\nPulse una tecla para seguir...");
	getch();
	printf("\n\n");
}
