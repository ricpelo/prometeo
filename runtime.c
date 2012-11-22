#include <alloc.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <dir.h>
#include <dos.h>
#include "Cod_Rt.H"
#include "Bandera.H"
#include "T_Objeto.H"
#include "L_Objeto.H"
#include "Pila.H"
#include "Vocab.H"
#include "Local.H"
#include "T_Mens.H"
#include "Sistema.H"
#include "T_Resp.H"
#include "T_Subr.H"
#include "Lexico.H"
#include "yytabin.h"
#include "Acciones.H"
#include "Runtime.H"
#include "aafli.h"
#include "Input.H"
#include "Fuentes.H"
#include "Detect.H"
#include "Xms.H"
#include "Smix.H"
#include "Interpre.H"
#include "Memoria.H"
#include "Fundido.H"

#define ON  1
#define OFF 0

#define TRUE  1
#define FALSE 0

#define SHAREDEMB    /* Undefine this to store all sounds in separate EMBs */
#undef SHAREDEMB
int baseio, irq, dma, dma16;
int haySonido;

int PararFli(int, int, int);
int PararFli2(int, int, int);
void LimpiarMemoria(void);
void Inicializa(int inicObj);
void DescribeSituacion(void);
int TomaCaracter(char *buffer, int *idx);
void DevuelveCaracter(int *idx);
void yyparse(void);

unsigned long TAM_CODIGO;
int terminal;                   /* Indica al parser si hay un "lo" */
char yytext[LONG_BUFFER];       /* El buffer de entrada del jugador */
char buffer[LONG_BUFFER];       /* El buffer de edici¢n */
extern int yylval;              /* Interfaz scanner <-> parser */
PILA *datos = NULL;             /* La pila de datos */
PILA *direcciones = NULL;       /* La pila de direcciones de subrutinas */
int bandera[TAM_BANDERA];       /* Las banderas */
CODIGO *codigo;                 /* El c¢digo generado, listo para ejecutar */
unsigned long pc;               /* El contador de programa */
int process1, process2;         /* Direcciones de las rutinas Process */
int timeout;                    /* Direcci¢n para la subrutina Timeout */
int hecho;                      /* Indica si se ha encontrado un HECHO */
int syntaxError;                /* Si ha habido un error sint ctico */
int pesoTotal;                  /* Peso actual de todo lo que se lleva */
int idx;                        /* Posici¢n dentro del buffer del jugador */


void ourexitproc(void)
{
	int i;

	for (i=0; i < numSonidos; ++i)
		if (sonidos[i] != NULL) free_sound(sonidos + i);
#ifdef SHAREDEMB
	shutdown_sharing();
#endif
}


void init(void)
{
	int i;
	char *p;
	struct ffblk fb;

	if (findfirst("SONIDOS.SND", &fb, 0) != -1) {
		haySonido = 1;
		if (!detect_settings(&baseio, &irq, &dma, &dma16)) {
			printf("Variable de entorno BLASTER err¢nea o no encontrado\n");
			printf("No podr  activarse el sonido\n");
			getch();
			haySonido = 0;
		} else {
			if (!init_sb(baseio, irq, dma, dma16)) {
				printf("Error inicializando tarjeta de sonido\n");
				printf("No podr  activarse el sonido\n");
				getch();
				haySonido = 0;
			}
		}

		if (haySonido) {
			if (!init_xms()) {
				printf("No puedo inicializar la memoria extendida\n");
				printf("Debe instalarse HIMEM.SYS\n");
				printf("No podr  activarse el sonido\n");
				getch();
				haySonido = 0;
			} else {
				if (!getfreexms()) {
					printf("Insuficiente memoria XMS libre\n");
					printf("No podr  activarse el sonido\n");
					getch();
					haySonido = 0;
				} else {
#ifdef SHAREDEMB
					init_sharing();
#endif
					atexit(ourexitproc);
				}
			}
			init_mixing();
		}
	} else
		haySonido = 0;

}


void shutdown(void)
{
	int i;

	shutdown_mixing();
	shutdown_sb();

	for (i = 0; i < numSonidos; i++)
		free_sound(sonidos + i);
#ifdef SHAREDEMB
	shutdown_sharing();
#endif
}


void main(int argc, char *argv[])
{
	char fich[MAXPATH],
		 drive[MAXDRIVE],
		 dir[MAXDIR],
		 file[MAXFILE],
		 ext[MAXEXT];
	FILE *fp;
	OBJETO *o;
	struct ffblk fb;
	int flag, valido;
	int n, i, opcMem = 0;
	unsigned long memoria;
	int verb, nomb;
	unsigned long idxCodigo;

	printf("PROMETEO 2.2.1 - Generador de Aventuras Conversacionales\n");
	printf("M¢dulo Run-Time (c) 1995-2001 Ricardo P‚rez L¢pez - Alpha Aventuras\n");

	if (argc == 1) {
		if (findfirst("*.GAM", &fb, 0) == -1) {
			printf("\n\tSintaxis: PR [fichero[.GAM]]\n");
			exit(1);
		} else {
			printf("(usar opci¢n /h o /? para sintaxis)\n\n");
			strcpy(fich, fb.ff_name);
		}

	} else {

		if (argv[1][0] == '/' || argv[1][0] == '-') {

			switch (argv[1][1]) {
				case 'h':
				case 'H':
				case '?':
					printf("\n\tSintaxis: PR [fichero[.GAM]]\n");
					exit(1);
					break;

				default:
					printf("\n\tOpci¢n desconocida\n");
					exit(1);

			}

		} else {
			if (argc == 3 && (argv[2][0] == '/' || argv[2][0] == '-'))

				switch (argv[2][1]) {
					case 'M':
					case 'm':
						opcMem = 1;
						break;
				}

			printf("(usar opci¢n /h o /? para sintaxis)\n\n");
			strcpy(fich, argv[1]);
		}

	}

	flag = fnsplit(fich, drive, dir, file, ext);
	if (!(flag & EXTENSION)) {
		strcpy(ext, ".GAM");
		fnmerge(fich, drive, dir, file, ext);
	}

	fp = fopen(fich, "rb");

	if (fp == NULL) {
		printf("El fichero \"%s\" no existe\n", fich);
		exit(1);
	}

	valido = 0;

	if (getc(fp) == 'P')
		if (getc(fp) == 'R')
			if (getc(fp) == 'O')
				valido = 1;

	if (!valido) {
		printf("El fichero \"%s\" no es un fichero PROMETEO\n", fich);
		exit(1);
	}

	init();                 /* Inicializa tarjeta de sonido */

	memoria = farcoreleft();
	/* Cargar las tablas de datos */
	printf("Leyendo datos. Por favor, espere...\n");
	LimpiarMemoria();
	CargarVocab(fp);
	CargarLocalidad(fp);
	CargarObjeto(fp);

	i = pesoTotal = 0;

	while (i < INDICE_OBJETO && (o = indiceObj[i]) != NULL) {
		if (o->inicial >= 0)
			MueveObjeto(i, o->inicial);

		i++;
	}

	CargarMensaje(fp);
	CargarSistema(fp);
	CargarRespuesta(fp);
	fread(&process1, sizeof process1, 1, fp);
	fread(&process2, sizeof process2, 1, fp);
	fread(&timeout,  sizeof timeout,  1, fp);

	fread(&TAM_CODIGO, sizeof TAM_CODIGO, 1, fp);
	codigo = (CODIGO *) Memoria(sizeof(CODIGO) * TAM_CODIGO);

	/* Cargar el vector de c¢digo */
	for (idxCodigo = 0; idxCodigo < TAM_CODIGO; idxCodigo++) {
		fread(&codigo[idxCodigo].codop, sizeof codigo[idxCodigo].codop, 1,
			  fp);
		fread(&codigo[idxCodigo].argumento,
			  sizeof codigo[idxCodigo].argumento, 1, fp);
	}

	fclose(fp);

	if (opcMem) {
		printf("Memoria inicial: %lu  - Memoria actual: %lu  - Diferencia: "
			   "%lu\n    Utilizaci¢n: %.1f %%", memoria, farcoreleft(),
			   memoria - farcoreleft(), (float)
			   ((100 * (memoria - farcoreleft())) / memoria));
		getch();
	}

	Fundido();
	randomize();
	Inicializa(0);                  /* Inicializa variables y pantalla */
	DescribeSituacion();

	while (1) {                     /* Bucle principal */

		if (process2 != -1 && !syntaxError) {   /* La subrutina Process2 */
			EmpujaPila(&direcciones, pc);
			pc = process2;
			hecho = 0;                  /* Para que el traductor no salga */
			Traductor();
		}

		/* Muestra el mensaje de "input" de entre los mensajes del sistema
		   2, 3, 4 y 5 en una proporci¢n 30:30:30:10. Si la bandera 42 est 
		   instanciada a un n§ de mensaje v lido, se imprimir  siempre ‚ste */
		if (bandera[42] != 0)
			Emite(sistema[bandera[42]]);
		else {
			i = random(100);
			Emite(sistema[(i < 30) ? 2 :
							(i >= 30 && i < 60) ? 3 :
							(i >= 60 && i < 90) ? 4 : 5]);
		}

		Emite(sistema[33]);             /* ">>>" */
		Input(buffer, LONG_BUFFER - 1, 1);
		idx = 0;
		yyparse();                      /* Pide input del jugador */
	}

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

	memset(tablaResp, NULL, sizeof tablaResp);
	memset(localObj, NULL, sizeof localObj);
}


/*
	Inicializa():   Da valores iniciales a las banderas, prepara pantalla,
					coloca los objetos en sus localidades iniciales
*/
void Inicializa(int inicObj)
{
	int i;
	OBJETO *o;
	struct time hora;

	_setcursortype(_SOLIDCURSOR);
	SetFont();

	for (i = 0; i < TAM_BANDERA; i++)
		if ((!inicObj && i != 1) || (inicObj && i != 59))
/*		if (i != 59 || !inicObj) */
			bandera[i] = 0;

	if (inicObj) {          /* Si se deben inicializar los objetos */
		pesoTotal = 0;      /* (la primera vez no es necesario) */
		i = 0;
		while (i < INDICE_OBJETO && (o = indiceObj[i]) != NULL) {
			MueveObjeto(i, o->inicial);
			i++;
		}
	}

	memset(sonidos, NULL, sizeof sonidos);
	bandera[37] = 4;        /* N§ m ximo de objetos llevables */
	bandera[52] = 10;       /* Peso m ximo permitido */
	gettime(&hora);
	bandera[20] = hora.ti_hour;
	bandera[21] = hora.ti_min;
	bandera[22] = hora.ti_sec;
	bandera[23] = hora.ti_hund;
}


/*
	DescribeSituacion():    Realiza las acciones de descripci¢n de la
							localidad actual
*/
void DescribeSituacion(void)
{
	int ivmode;
	Errval err;
	char pantalla[MAXFILE + MAXEXT];
	int oscuro, luzAusente;
	int p, top, bottom;
	int loc = DarActualObjeto(DarObjeto(0));

	oscuro = bandera[0] != 0;
	luzAusente = loc != bandera[38] && loc != ENCIMA && loc != LLEVADO;

	if (!ExisteLocalidad(bandera[38])) {
		EmiteEnteroError("ERROR: La localidad _ no existe", bandera[38]);
		return;
	}

	if (bandera[2] > 0)
		bandera[2]--;

	if (oscuro) {
		if (bandera[3] > 0)
			bandera[3]--;
		if (bandera[4] > 0 && luzAusente)
			bandera[4]--;
	}

	if (bandera[40] == 0) {         /* b40 controla el borrado de pantalla */
		window(1, 1, 80, 25);
		clrscr();
	} else if (bandera[40] == 1)
		clrscr();

	if (oscuro && luzAusente)
		Emite(sistema[0]);          /* "Est  muy oscuro" */
	else {
		p = PantallaLocalidad(bandera[38]);

		if (p != -1 && bandera[29] == 0) {
			strcpy(pantalla, DescripcionMensaje(p));
			ivmode = dos_get_vmode();
			dos_set_vmode(0x13);

			if (dos_get_vmode() == 0x13) {
				err = fli_until(pantalla, -1, PararFli);
				getch();
				dos_set_vmode(ivmode);
				_setcursortype(_SOLIDCURSOR);
				SetFont();

				if (err < AA_SUCCESS)
					EmiteLn(fli_error_message(err));

			} else
				EmiteError("ERROR: La tarjeta gr fica no es una VGA/MCGA");

		}

		Emite(DescripcionLocalidad(bandera[38]));
		Emite(" ");
	}

	if (process1 != -1) {               /* La subrutina Process1 */
		EmpujaPila(&direcciones, pc);
		pc = process1;
		Traductor();
	}

}


/*
	yylex():    El analizador l‚xico de la entrada del usuario
	Devuelve:   El token encontrado en la entrada
*/
int yylex(void)
{
	int c, pos, i, j;
	int salida;
	PALABRA *palabra;
	static char *term[] = { "LA", "LAS", "LE", "LES", "LO", "LOS", "-" };

	while (1) {
		pos = 0;

		while ((c = TomaCaracter(buffer, &idx)) == ' ' || c == '\t') /* Filtra espacios */
			;

		if (EsAlfanum(c)) {

			do {
				if (pos < LONG_BUFFER - 1)   /* S¢lo los primeros caracteres */
					yytext[pos++] = toupper(c);
				c = TomaCaracter(buffer, &idx);
			} while (EsAlfanum(c));

			yytext[pos] = '\0';
			DevuelveCaracter(&idx);
			palabra = DarPalabraVocab(yytext);

			if (palabra == NULL) {

				for (i = strlen(yytext); i >= 0; i--) {
					j = 0;

					while ((salida = strcmp(yytext + i, term[j])) > 0 &&
							*term[j] != '-')
						j++;

					if (salida == 0) {
						yytext[i] = '\0';
						palabra = DarPalabraVocab(yytext);
						terminal = 1;
					}
				}

				palabra = DarPalabraVocab(yytext);
			}

			if (palabra != NULL) {
				yylval = CodigoVocab(yytext);

				switch (palabra->tipo) {
					case T_VERBO:       return VERBO;
					case T_ADVERBIO:    return ADVERBIO;
					case T_NOMBRE:      return NOMBRE;
					case T_ADJETIVO:    return ADJETIVO;
					case T_PREPOSICION: return PREPOSICION;
					case T_CONJUNCION:  return CONJUNCION;
					case T_PRONOMBRE:   return PRONOMBRE;
					case T_NOMBRE_VERBO: return NOMBRE_VERBO;
					case T_DIRECCION: return DIRECCION;
				}
			}

		} else
			/* Otro car cter de puntuaci¢n */
			return (c == '\n' || c == '\0') ? EOF : c;

	}
}


int TomaCaracter(char *buffer, int *idx)
{
	int salida = buffer[*idx];
	*idx = *idx + 1;
	return salida;
}


void DevuelveCaracter(int *idx)
{
	*idx = *idx - 1;
}


/*
	Traductor():    La funci¢n que recorre el vector de c¢digo ejecut ndolo
*/
void Traductor(void)
{
	int valor, valor2, direc;
	int c, yylval;
	int subrutina = 1;
	int ivmode;
	Errval err;
	char *p;
	int nombre, adjetivo, salir;
	int valor3, valor4;
	OBJETO *obj;

	do {
		yylval = codigo[pc].argumento;
		c = codigo[pc++].codop;

		switch(c) {
			case EMPUJA:
				EmpujaPila(&datos, yylval);
				break;

			case CARGA:
				direc = SacaPila(&datos);
				EmpujaPila(&datos, bandera[direc]);
				break;

			case ALMACENA:
				valor = SacaPila(&datos);
				direc = SacaPila(&datos);
				bandera[direc] = valor;
				break;

			case SALTASBR:
				subrutina++;
				EmpujaPila(&direcciones, pc);
				pc = yylval;
				break;

			case RETSBR:
				subrutina = (subrutina > 0) ? subrutina - 1 : 0;
				pc = SacaPila(&direcciones);
				break;

			case SALTAFALSO:
				valor = SacaPila(&datos);
				if (valor == 0)
					pc = yylval;
				break;

			case SALTA:
				pc = yylval;
				break;

			case C_MENOSU:
				valor = SacaPila(&datos);
				EmpujaPila(&datos, -valor);
				break;

			case C_NOT:
				valor = SacaPila(&datos);
				EmpujaPila(&datos, !valor);
				break;

			case C_AND:
				valor = SacaPila(&datos);
				valor2 = SacaPila(&datos);
				EmpujaPila(&datos, valor2 && valor);
				break;

			case C_OR:
				valor = SacaPila(&datos);
				valor2 = SacaPila(&datos);
				EmpujaPila(&datos, valor2 || valor);
				break;

			case '+':
				valor = SacaPila(&datos);
				valor2 = SacaPila(&datos);
				EmpujaPila(&datos, valor2 + valor);
				break;

			case '-':
				valor = SacaPila(&datos);
				valor2 = SacaPila(&datos);
				EmpujaPila(&datos, valor2 - valor);
				break;

			case '*':
				valor = SacaPila(&datos);
				valor2 = SacaPila(&datos);
				EmpujaPila(&datos, valor2 * valor);
				break;

			case '/':
				valor = SacaPila(&datos);
				valor2 = SacaPila(&datos);
				EmpujaPila(&datos, valor2 / valor);
				break;

			case C_MODULO:
				valor = SacaPila(&datos);
				valor2 = SacaPila(&datos);
				EmpujaPila(&datos, valor2 % valor);
				break;

			case C_MAY:
				valor = SacaPila(&datos);
				valor2 = SacaPila(&datos);
				EmpujaPila(&datos, valor2 > valor);
				break;

			case C_MEN:
				valor = SacaPila(&datos);
				valor2 = SacaPila(&datos);
				EmpujaPila(&datos, valor2 < valor);
				break;

			case C_MAI:
				valor = SacaPila(&datos);
				valor2 = SacaPila(&datos);
				EmpujaPila(&datos, valor2 >= valor);
				break;

			case C_MEI:
				valor = SacaPila(&datos);
				valor2 = SacaPila(&datos);
				EmpujaPila(&datos, valor2 <= valor);
				break;

			case C_EQU:
				valor = SacaPila(&datos);
				valor2 = SacaPila(&datos);
				EmpujaPila(&datos, valor2 == valor);
				break;

			case C_NEQ:
				valor = SacaPila(&datos);
				valor2 = SacaPila(&datos);
				EmpujaPila(&datos, valor2 != valor);
				break;

			case C_OBJETO:
				valor = SacaPila(&datos);
				valor2 = SacaPila(&datos);
				valor3 = CodigoObjeto(valor2, valor);
				obj = DarObjeto(valor3);

				if (obj == NULL) {
					valor3 = Siguiente(ENCIMA_LLEVADO, 0);
					DarNombreObjeto(DarObjeto(valor3), &nombre, &adjetivo);
					while (valor3 != -1 && nombre != bandera[34]) {
						valor3 = Siguiente(ENCIMA_LLEVADO, 1);
						DarNombreObjeto(DarObjeto(valor3), &nombre,
										&adjetivo);
					}

					if (valor3 == -1) {
						valor3 = Siguiente(bandera[38], 0);
						DarNombreObjeto(DarObjeto(valor3), &nombre,
										&adjetivo);
						while (valor3 != -1 && nombre != bandera[34]) {
							valor3 = Siguiente(bandera[38], 1);
							DarNombreObjeto(DarObjeto(valor3), &nombre,
											&adjetivo);
						}

						if (valor3 != -1)
							obj = DarObjeto(valor3);

					} else
						obj = DarObjeto(valor3);
				}

				valor4 = DarTipoObjeto(obj);
				bandera[54] = DarActualObjeto(obj);
				bandera[55] = DarPesoObjeto(obj);
				bandera[56] = valor4 == T_CONTENEDOR;
				bandera[57] = valor4 == T_ROPA;
				bandera[58] = DarCodigoObjeto(obj);
				EmpujaPila(&datos, DarCodigoObjeto(obj));
				break;

			case C_OBJETO_DEFECTO:
				valor3 = CodigoObjeto(bandera[34], bandera[35]);
				obj = DarObjeto(valor3);

				if (obj == NULL) {
					valor3 = Siguiente(ENCIMA_LLEVADO, 0);
					DarNombreObjeto(DarObjeto(valor3), &nombre, &adjetivo);
					while (valor3 != -1 && nombre != bandera[34]) {
						valor3 = Siguiente(ENCIMA_LLEVADO, 1);
						DarNombreObjeto(DarObjeto(valor3), &nombre,
										&adjetivo);
					}

					if (valor3 == -1) {
						valor3 = Siguiente(bandera[38], 0);
						DarNombreObjeto(DarObjeto(valor3), &nombre,
										&adjetivo);
						while (valor3 != -1 && nombre != bandera[34]) {
							valor3 = Siguiente(bandera[38], 1);
							DarNombreObjeto(DarObjeto(valor3), &nombre,
											&adjetivo);
						}

						if (valor3 != -1)
							obj = DarObjeto(valor3);

					} else
						obj = DarObjeto(valor3);
				}

				valor4 = DarTipoObjeto(obj);
				bandera[54] = DarActualObjeto(obj);
				bandera[55] = DarPesoObjeto(obj);
				bandera[56] = valor4 == T_CONTENEDOR;
				bandera[57] = valor4 == T_ROPA;
				bandera[58] = DarCodigoObjeto(obj);
				EmpujaPila(&datos, DarCodigoObjeto(obj));
				break;

			case C_ESTA:
				valor = SacaPila(&datos);
				EmpujaPila(&datos, bandera[38] == valor);
				break;

			case C_ESTAMAY:
				valor = SacaPila(&datos);
				EmpujaPila(&datos, bandera[38] > valor);
				break;

			case C_ESTAMEN:
				valor = SacaPila(&datos);
				EmpujaPila(&datos, bandera[38] < valor);
				break;

			case C_PRESENTE:
				valor = SacaPila(&datos);
				valor2 = DarActualObjeto(DarObjeto(valor));
				EmpujaPila(&datos, valor2 == bandera[38] ||
						   valor2 == ENCIMA || valor2 == LLEVADO);
				break;

			case C_AUSENTE:
				valor = SacaPila(&datos);
				valor2 = DarActualObjeto(DarObjeto(valor));
				EmpujaPila(&datos, valor2 != bandera[38] &&
						   valor2 != ENCIMA && valor2 != LLEVADO);
				break;

			case C_PUESTO:
				valor = SacaPila(&datos);
				EmpujaPila(&datos, DarActualObjeto(DarObjeto(valor)) ==
						   ENCIMA);
				break;

			case C_LLEVADO:
				valor = SacaPila(&datos);
				EmpujaPila(&datos, DarActualObjeto(DarObjeto(valor)) ==
						   LLEVADO);
				break;

			case C_ESTAEN:
				valor = SacaPila(&datos);
				valor2 = SacaPila(&datos);
				EmpujaPila(&datos, DarActualObjeto(DarObjeto(valor2)) ==
						   valor);
				break;

			case C_LOCAL:
				valor = SacaPila(&datos);
				EmpujaPila(&datos, DarActualObjeto(DarObjeto(valor)));
				break;

			case C_VERB1:
				valor = SacaPila(&datos);
				EmpujaPila(&datos, valor == bandera[33]);
				break;

			case C_DIRECC1:
				valor = SacaPila(&datos);
				EmpujaPila(&datos, DarTipoPalabra(PrimeraVocab(valor)) ==
						   T_DIRECCION);
				break;

			case C_NOMB1:
				valor = SacaPila(&datos);
				EmpujaPila(&datos, valor == bandera[34]);
				break;

			case C_ADJET1:
				valor = SacaPila(&datos);
				EmpujaPila(&datos, valor == bandera[35]);
				break;

			case C_ADVERB:
				valor = SacaPila(&datos);
				EmpujaPila(&datos, valor == bandera[36]);
				break;

			case C_PREPOS:
				valor = SacaPila(&datos);
				EmpujaPila(&datos, valor == bandera[43]);
				break;

			case C_NOMB2:
				valor = SacaPila(&datos);
				EmpujaPila(&datos, valor == bandera[44]);
				break;

			case C_ADJET2:
				valor = SacaPila(&datos);
				EmpujaPila(&datos, valor == bandera[45]);
				break;

			case C_ACABAR:
				EmpujaPila(&datos, Acabar1());
				break;

			case C_ANALIZA:
				bandera[33] = bandera[48];  /* Cambia Verbo de sentencia */
				bandera[34] = bandera[49];  /* Cambia Nombre de sentencia */
				bandera[35] = bandera[50];  /* Cambia Adjetivo de sentencia */
				EmpujaPila(&datos, bandera[48] != -1);
				break;

			case C_ALEATORIO:
				valor = SacaPila(&datos);
				EmpujaPila(&datos, random(valor));
				break;

			case C_PRIMERO:
				valor = SacaPila(&datos);
				EmpujaPila(&datos, SiguienteObjeto(valor, 0));
				break;

			case C_SIGUIENTE:
				valor = SacaPila(&datos);
				EmpujaPila(&datos, SiguienteObjeto(valor, 1));
				break;

			case C_PESO:
				valor = SacaPila(&datos);
				EmpujaPila(&datos, DarPesoObjeto(DarObjeto(valor)));
				break;

			case C_CONTENEDOR:
				valor = SacaPila(&datos);
				EmpujaPila(&datos, DarTipoObjeto(DarObjeto(valor)) ==
					T_CONTENEDOR);
				break;

			case C_ROPA:
				valor = SacaPila(&datos);
				EmpujaPila(&datos, DarTipoObjeto(DarObjeto(valor)) ==
					T_ROPA);
				break;

			case C_POSX:
				EmpujaPila(&datos, wherex());
				break;

			case C_POSY:
				EmpujaPila(&datos, wherey());
				break;

			case C_CAMINAR:
				valor = SacaPila(&datos);
				valor2 = SacaPila(&datos);
				if (!ExisteLocalidad(valor2))
					EmiteEnteroError("ERROR: La localidad _ no existe", valor2);
				else
					EmpujaPila(&datos, Caminar(valor2, valor));
				break;

			case A_ACABAR:
				if (Acabar()) {
					Fundido();
					Inicializa(1);
					DescribeSituacion();
				} else {
					dos_set_vmode(dos_get_vmode()); /* Restablecer modo */
					exit(0);
				}
				break;

			case A_COGER:
				valor = SacaPila(&datos);
				if (DarObjeto(valor) == NULL)
					EmiteError("ERROR: El objeto no existe");
				else
					hecho = Coger(valor);
				break;

			case A_DEJAR:
				valor = SacaPila(&datos);
				if (DarObjeto(valor) == NULL)
					EmiteError("ERROR: El objeto no existe");
				else
					hecho = Dejar(valor);
				break;

			case A_PONER:
				valor = SacaPila(&datos);
				if (DarObjeto(valor) == NULL)
					EmiteError("ERROR: El objeto no existe");
				else
					hecho = Poner(valor);
				break;

			case A_QUITAR:
				valor = SacaPila(&datos);
				if (DarObjeto(valor) == NULL)
					EmiteError("ERROR: El objeto no existe");
				else
					hecho = Quitar(valor);
				break;

			case A_CREAR:
				valor = SacaPila(&datos);
				if (DarObjeto(valor) == NULL)
					EmiteError("ERROR: El objeto no existe");
				else
					MueveObjeto(valor, bandera[38]);
				break;

			case A_DESTRUIR:
				valor = SacaPila(&datos);
				if (DarObjeto(valor) == NULL)
					EmiteError("ERROR: El objeto no existe");
				else
					MueveObjeto(valor, NO_CREADO);
				break;

			case A_COLOCAR:
				valor = SacaPila(&datos);
				valor2 = SacaPila(&datos);
				if (!ExisteLocalidad(valor))
					EmiteEnteroError("ERROR: La localidad _ no existe", valor);
				else if (DarObjeto(valor2) == NULL)
					EmiteError("ERROR: El objeto no existe");
				else
					MueveObjeto(valor2, valor);
				break;

			case A_INTERCAMBIAR:
				valor = SacaPila(&datos);
				valor2 = SacaPila(&datos);
				if (DarObjeto(valor) == NULL || DarObjeto(valor2) == NULL)
					EmiteError("ERROR: El objeto no existe");
				else
					Intercambiar(valor2, valor);
				break;

			case A_METER:
				valor = SacaPila(&datos);
				valor2 = SacaPila(&datos);
				if (DarObjeto(valor) == NULL || DarObjeto(valor2) == NULL)
					EmiteError("ERROR: El objeto no existe");
				else
					hecho = Meter(valor, valor2);
				break;

			case A_SACAR:
				valor = SacaPila(&datos);
				valor2 = SacaPila(&datos);
				if (DarObjeto(valor) == NULL || DarObjeto(valor2) == NULL)
					EmiteError("ERROR: El objeto no existe");
				else
					hecho = Sacar(valor2, valor);
				break;

			case A_AUTOC:
				if (bandera[58] == -1) {
					valor = Siguiente(bandera[38], 0);
					DarNombreObjeto(DarObjeto(valor), &nombre, &adjetivo);
					while (valor != -1 && nombre != bandera[34]) {
						valor = Siguiente(bandera[38], 1);
						DarNombreObjeto(DarObjeto(valor), &nombre, &adjetivo);
					}

					if (valor != -1) {
						bandera[54] = bandera[38];
						bandera[55] = DarPesoObjeto(DarObjeto(valor));
						bandera[56] = DarTipoObjeto(DarObjeto(valor))
									  == T_CONTENEDOR;
						bandera[57] = DarTipoObjeto(DarObjeto(valor))
									  == T_ROPA;
						bandera[58] = valor;
					}

					hecho = Coger(valor);
				} else
					hecho = Coger(bandera[58]);
				break;

			case A_AUTOD:
				if (bandera[58] == -1) {
					valor = Siguiente(LLEVADO, 0);
					DarNombreObjeto(DarObjeto(valor), &nombre, &adjetivo);
					while (valor != -1 && nombre != bandera[34]) {
						valor = Siguiente(LLEVADO, 1);
						DarNombreObjeto(DarObjeto(valor), &nombre, &adjetivo);
					}

					if (valor != -1) {
						bandera[54] = LLEVADO;
						bandera[55] = DarPesoObjeto(DarObjeto(valor));
						bandera[56] = DarTipoObjeto(DarObjeto(valor))
									  == T_CONTENEDOR;
						bandera[57] = DarTipoObjeto(DarObjeto(valor))
									  == T_ROPA;
						bandera[58] = valor;
					}

					hecho = Dejar(valor);
				} else
					hecho = Dejar(bandera[58]);
				break;

			case A_AUTOP:
				if (bandera[58] == -1) {
					valor = Siguiente(LLEVADO, 0);
					DarNombreObjeto(DarObjeto(valor), &nombre, &adjetivo);
					while (valor != -1 && nombre != bandera[34]) {
						valor = Siguiente(LLEVADO, 1);
						DarNombreObjeto(DarObjeto(valor), &nombre, &adjetivo);
					}
					if (valor != -1) {
						bandera[54] = LLEVADO;
						bandera[55] = DarPesoObjeto(DarObjeto(valor));
						bandera[56] = DarTipoObjeto(DarObjeto(valor))
									  == T_CONTENEDOR;
						bandera[57] = DarTipoObjeto(DarObjeto(valor))
									  == T_ROPA;
						bandera[58] = valor;
					}

					hecho = Poner(valor);
				} else
					hecho = Poner(bandera[58]);
				break;

			case A_AUTOQ:
				if (bandera[58] == -1) {
					valor = Siguiente(ENCIMA, 0);
					DarNombreObjeto(DarObjeto(valor), &nombre, &adjetivo);
					while (valor != -1 && nombre != bandera[34]) {
						valor = Siguiente(ENCIMA, 1);
						DarNombreObjeto(DarObjeto(valor), &nombre, &adjetivo);
					}

					if (valor != -1) {
						bandera[54] = ENCIMA;
						bandera[55] = DarPesoObjeto(DarObjeto(valor));
						bandera[56] = DarTipoObjeto(DarObjeto(valor))
									  == T_CONTENEDOR;
						bandera[57] = DarTipoObjeto(DarObjeto(valor))
									  == T_ROPA;
						bandera[58] = valor;
					}

					hecho = Quitar(valor);
				} else
					hecho = Quitar(bandera[58]);
				break;


			case A_AUTOM:
				if (bandera[58] == -1) {
					valor = Siguiente(LLEVADO, 0);
					DarNombreObjeto(DarObjeto(valor), &nombre, &adjetivo);
					while (valor != -1 && nombre != bandera[34]) {
						valor = Siguiente(LLEVADO, 1);
						DarNombreObjeto(DarObjeto(valor), &nombre, &adjetivo);
					}

					if (valor != -1) {
						bandera[54] = LLEVADO;
						bandera[55] = DarPesoObjeto(DarObjeto(valor));
						bandera[56] = DarTipoObjeto(DarObjeto(valor))
									  == T_CONTENEDOR;
						bandera[57] = DarTipoObjeto(DarObjeto(valor))
									  == T_ROPA;
						bandera[58] = valor;
					}

					hecho = Meter(SacaPila(&datos), valor);
				} else
					hecho = Meter(SacaPila(&datos), bandera[58]);
				break;

			case A_AUTOS:
				valor2 = SacaPila(&datos);
				if (bandera[58] == -1) {
					valor = Siguiente(valor2, 0);
					DarNombreObjeto(DarObjeto(valor), &nombre, &adjetivo);
					while (valor != -1 && nombre != bandera[34]) {
						valor = Siguiente(valor2, 1);
						DarNombreObjeto(DarObjeto(valor), &nombre, &adjetivo);
					}

					if (valor != -1) {
						bandera[54] = valor2;
						bandera[55] = DarPesoObjeto(DarObjeto(valor));
						bandera[56] = DarTipoObjeto(DarObjeto(valor))
									  == T_CONTENEDOR;
						bandera[57] = DarTipoObjeto(DarObjeto(valor))
									  == T_ROPA;
						bandera[58] = valor;
					}

					hecho = Sacar(valor2, valor);
				} else
					hecho = Sacar(valor2, bandera[58]);
				break;

			case A_TURNOS:
				Turnos();
				break;

			case A_LISTAOBJ:
				ListaObj(bandera[38]);
				break;

			case A_LISTADE:
				valor = SacaPila(&datos);
				if (!ExisteLocalidad(valor))
					EmiteEnteroError("ERROR: La localidad _ no existe", valor);
				else
					ListaDe(valor);
				break;

			case A_LISTACONT:
				valor = SacaPila(&datos);
				if (DarObjeto(valor) == NULL)
					EmiteError("ERROR: El objeto no existe");
				else if (DarTipoObjeto(obj = DarObjeto(valor)) !=
						 T_CONTENEDOR)
					EmiteError("ERROR: El objeto no es un contenedor");
				else
					ListaDe(DarAlmacenObjeto(obj));
				break;

			case A_INVEN:
				Inventario();
				break;

			case A_MENS:
				EmiteCadena(DescripcionMensaje(SacaPila(&datos)),
							DarDescripcionObjeto(DarObjeto(bandera[58])));
				break;

			case A_MENSIS:
				valor = SacaPila(&datos);
				if (valor >= 0 && valor < NUM_SISTEMA)
					EmiteCadena(DescripcionSistema(valor),
								DarDescripcionObjeto(DarObjeto(bandera[58])));
				else
					EmiteEnteroError("ERROR: El mensaje de sistema _ no existe",
								  valor);
				break;

			case A_IMPRIME:
				EmiteEntero("_", SacaPila(&datos));
				break;

			case A_TECLA:
				Tecla();
				break;

			case A_PAUSA:
				delay(SacaPila(&datos));
				break;

			case A_DESCRIBE:
				DescribeSituacion();
				break;

			case A_MUEVE:
				valor = SacaPila(&datos);
				if (ExisteLocalidad(valor))
					bandera[38] = valor;
				else
					EmiteEnteroError("ERROR: La localidad _ no existe", valor);
				break;

			case A_DIAG:
				Diagnostico();
				hecho = 1;
				break;

			case A_HECHO:
				hecho = 1;
				break;

			case A_NO_HECHO:
				Emite(sistema[8]);      /* "No puedo hacer eso" */
				hecho = 1;
				break;

			case A_SALVAR:
				SalvarSituacion();
				hecho = 1;
				break;

			case A_CARGAR:
				if (CargarSituacion()) {
					DescribeSituacion();
					hecho = 1;
				}
				break;

			case A_BORRAR:
				window(1, 1, 80, 25);
				clrscr();
				break;

			case A_SALIDAS:
				Salidas();
				break;

			case A_CURSOR:
				valor = SacaPila(&datos);
				valor2 = SacaPila(&datos);
				gotoxy(valor2, valor);
				break;

			case A_SALTOLINEA:
				cprintf("\r\n");
				break;

			case A_IMAGEN:
				if (bandera[29] == 0) {
					p = DescripcionMensaje(SacaPila(&datos));
					ivmode = dos_get_vmode();
					dos_set_vmode(0x13);            /* 0x13 */

					if (dos_get_vmode() == 0x13) {  /* 0x13 */
						err = fli_until(p, -1, PararFli);
						getch();
						dos_set_vmode(ivmode);
						_setcursortype(_SOLIDCURSOR);
						SetFont();

						if (err < AA_SUCCESS)
							EmiteLn(fli_error_message(err));

					} else
						EmiteError("ERROR: La tarjeta gr fica no es una VGA/MCGA");
				}

				break;

			case A_IMAGEN2:
				if (bandera[29] == 0) {
					p = DescripcionMensaje(SacaPila(&datos));
					ivmode = dos_get_vmode();
					dos_set_vmode(0x13);            /* 0x13 */

					if (dos_get_vmode() == 0x13) {  /* 0x13 */
						err = fli_until(p, -1, PararFli2);
						getch();
						dos_set_vmode(ivmode);
						_setcursortype(_SOLIDCURSOR);
						SetFont();

						if (err < AA_SUCCESS)
							EmiteLn(fli_error_message(err));

					} else
						EmiteError("ERROR: La tarjeta gr fica no es una VGA/MCGA");
				}

				break;

			case A_SONIDO:
				if (haySonido) {
					p = DescripcionMensaje(SacaPila(&datos));
					Sonido(p, 0);
				}
				break;

			case A_SONIDO2:
				if (haySonido) {
					p = DescripcionMensaje(SacaPila(&datos));
					Sonido(p, 1);
				}
				break;

			case A_SILENCIO:
				if (haySonido) {
					p = DescripcionMensaje(SacaPila(&datos));
					Silencio(p);
				}
				break;

			case A_VENTANA:
				valor = SacaPila(&datos);
				valor2 = SacaPila(&datos);
				window(1, valor2, 80, valor);
				break;

			case A_COLOR:
				textcolor(SacaPila(&datos));
				break;

			case A_FONDO:
				textbackground(SacaPila(&datos));
				break;

			case A_FUENTE:
				p = DescripcionMensaje(SacaPila(&datos));
				LoadFileFont(p);
				break;

		}

		if (hecho) {
			subrutina = (subrutina > 0) ? subrutina - 1 : 0;
			pc = SacaPila(&direcciones);
		}

	} while (!(c == RETSBR && subrutina == 0) && !hecho);

}

#pragma argsused

int PararFli(int actual, int total, int veces)
{
	if ((actual >= total) || kbhit())
		return 0;
	else
		return 1;
}

#pragma argsused

int PararFli2(int actual, int total, int veces)
{
	return !kbhit();
}
