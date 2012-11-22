#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dir.h>
#include <alloc.h>
#include "Input.H"
#include "Bandera.H"
#include "Acciones.H"
#include "Sistema.H"
#include "T_Objeto.H"
#include "L_Objeto.H"
#include "Local.H"
#include "Runtime.H"
#include "Lexico.H"				/* Para LONG_BUFFER */
#include "Vocab.H"
#include "Detect.H"
#include "Xms.H"
#include "Memoria.H"

typedef struct ListaSonidos {
	char *ident;
	int indice;
	struct ListaSonidos *siguiente;
} LISTASONIDOS;

static LISTASONIDOS *listasonidos = NULL;
SOUND *sonidos[NUM_SONIDOS];
int numSonidos = 0;

static int BuscaSonido(char *p);
static int IncluirSonido(char *p, int i);
static void BorrarSonido(int i);
static void ListaDeObjetos(int loc, int sw);
static LOBJETO *SgteAux(LOBJETO *org);
static char *Token(char *cadena, char *sep);

int Longitud(char *s)
{
	int i, j, lon = 0;
	int c;
	char buffer[4];
	char result[6];
	int idx;

	for (i = 0; s[i]; i++)

		if ((c = s[i]) != '\\')
			lon += (c == '\t') ? 4 : 1;
		else {

			if ((c = s[++i]) == 'n')
				lon++;
			else if (c == 't')
				lon++;
			else if (c == 'b') {
				j = 0;
				c = s[++i];

				while (isdigit(c)) {
					buffer[j++] = c;
					c = s[i + j];
				}

				i += j - 1;
				idx = atoi(buffer);
				itoa(idx, result, 10);
				lon += strlen(result);
			} else if (c == '\\')
				lon++;
			else if (c == '\'' || c == '\"')
				lon++;
			else if (isdigit(c)) {
				j = 0;

				while (isdigit(c)) {
					j++;
					c = s[i + j];
				}

				i += j - 1;
			} else if (c == 'f') {
				j = 0;
				c = s[++i];

				while (isdigit(c))
					c = s[i + j];

				i += j - 1;
			}

		}

	return lon;
}


void Emite(char *s)
{
	int i, j, c;
	static char separador[] = { '\n', '\t', ' ', '\0' };
	char salida[LONG_BUFFER];
	char *p;
	char buffer[4];
	int idx;
	int lon;

	strncpy(salida, s, LONG_BUFFER - 1);
	salida[LONG_BUFFER - 1] = '\0';
	p = Token(salida, separador);

	if (((lon = Longitud(p)) + wherex() >= 79) && lon > 1)
		cprintf("\r\n");

	for (i = 0; s[i]; i++)

		if ((c = s[i]) != '\\') {

			if (c == '\n' || c == '\t' || c == ' ') {
				if (c == '\n')
					cprintf("\r\n");
				else
					if (c != '\t')
						putch(c);
					else
						cprintf("    ");
				p = Token(NULL, separador);

				if (Longitud(p) + wherex() >= 79)
					cprintf("\r\n");

			} else
				putch(c);

		} else {

			if ((c = s[++i]) == 'n') {
				cprintf("\r\n");
				p = Token(NULL, separador);

				if (Longitud(p) + wherex() >= 79)
					cprintf("\r\n");

			} else if (c == 't') {
				putch('\t');
				p = Token(NULL, separador);

				if (Longitud(p) + wherex() >= 79)
					cprintf("\r\n");

			} else if (c == 'b') {
				j = 0;
				c = s[++i];

				while (isdigit(c)) {
					buffer[j++] = c;
					c = s[i + j];
				}

				i += j - 1;
				buffer[j] = '\0';
				idx = atoi(buffer);
				cprintf("%d", (idx >= 0 && idx <= TAM_BANDERA) ?
							  bandera[idx] : -1);
				p = Token(NULL, separador);

				if (Longitud(p) + wherex() >= 79)
					cprintf("\r\n");

			} else if (c == '\\') {
				putch('\\');
				p = Token(NULL, separador);

				if (Longitud(p) + wherex() >= 79)
					cprintf("\r\n");

			} else if (c == '\'') {
				putch('\"');
				p = Token(NULL, separador);

				if (Longitud(p) + wherex() >= 79)
					cprintf("\r\n");

			} else if (isdigit(c)) {
				j = 0;

				while (isdigit(c)) {
					buffer[j++] = c;
					c = s[i + j];
				}

				i += j - 1;
				buffer[j] = '\0';
				textcolor(atoi(buffer));
			} else if (c == 'f') {
				j = 0;
				c = s[++i];

				while (isdigit(c)) {
					buffer[j++] = c;
					c = s[i + j];
				}

				i += j - 1;
				buffer[j] = '\0';
				textbackground(atoi(buffer));
			}

		}

}


static char *Token(char *cadena, char *sep)
{
	static char *inic;
	char *p, *q, *s;

	if (cadena != NULL)
		inic = cadena;

	p = inic;

	while (p != NULL && *p != '\0') {

		for (s = sep; *s; s++)

			if (*p == *s) {
				*p = '\0';
				q = inic;
				inic = p + 1;
				return q;
			}

		p++;
	}

	return inic;			/* Antigua return cadena */
}


void EmiteLn(char *s)
{
	Emite(s);
	cprintf("\r\n");
}


void EmiteEntero(char *cadena, int valor)
{
	int i, j, c;
	char buffer[LONG_BUFFER];
	char entero[6];

	memset(entero, 0, sizeof entero);
	strcpy(buffer, cadena);

	for (i = 0, j = 0; cadena[i]; i++, j++)
		if ((c = cadena[i]) != '_')
			buffer[j] = c;
		else {
			sprintf(entero, "%d", valor);
			sprintf(buffer + j, "%d", valor);
			j += strlen(entero) - 1;
		}

	buffer[j] = '\0';
	Emite(buffer);
}


void EmiteEnteroLn(char *cadena, int valor)
{
	EmiteEntero(cadena, valor);
	cprintf("\r\n");
}


void EmiteCadena(char *cadena, char *valor)
{
	int i, j, c;
	char buffer[LONG_BUFFER], buffer2[LONG_BUFFER];

	strcpy(buffer2, cadena);

	for (i = 0, j = 0; buffer2[i]; i++, j++)
		if ((c = buffer2[i]) != '_')
			buffer[j] = c;
		else {
			sprintf(buffer + j, "%s", valor);
			j += strlen(valor) - 1;
		}

	buffer[j] = '\0';
	buffer[0] = toupper(buffer[0]);
	Emite(buffer);
}


void EmiteCadenaLn(char *cadena, char *valor)
{
	EmiteCadena(cadena, valor);
	cprintf("\r\n");
}


void EmiteCadenaError(char *cadena, char *valor)
{
	EmiteCadenaLn(cadena, valor);
	getch();
}


void EmiteEnteroError(char *cadena, int valor)
{
	EmiteEnteroLn(cadena, valor);
	getch();
}


void EmiteError(char *cadena)
{
	EmiteLn(cadena);
	getch();
}


int Acabar1(void)
{
	int result;
	char buffer[8];
	int anterior = bandera[11];

	Emite(sistema[12]);
	Emite(sistema[33]);
	bandera[11] = 0;
	Input(buffer, 7, 1);
	bandera[11] = anterior;

	if ((result = toupper(buffer[0]) == sistema[30][0]) == 1)
		Turnos();

	return result;
}


int Acabar(void)
{
	char buffer[8];
	int anterior = bandera[11];

	bandera[11] = 0;
	Emite(sistema[13]);
	Emite(sistema[33]);
	Input(buffer, 7, 1);
	bandera[11] = anterior;

	return toupper(buffer[0]) == sistema[31][0];
}


int Coger(int o)
{
	OBJETO *obj = DarObjeto(o);
	int loc = DarActualObjeto(obj);
	char *desc = DarDescripcionObjeto(obj);

	if (obj == NULL) {
		Emite(sistema[26]);					/* "No hay eso aqu¡" */
		return 1;
	} else if (loc == LLEVADO || loc == ENCIMA) {
		EmiteCadena(sistema[25], desc);    /* "Ya tengo _" */
		return 1;
	} else if (loc != bandera[38]) {
		Emite(sistema[26]);			/* "No hay eso aqu¡" */
		return 1;
	} else if (bandera[52] >= 0 &&
			   (pesoTotal + DarPesoObjeto(obj) > bandera[52])) {
		EmiteCadena(sistema[43], desc);/* "Pesa mucho" */
		return 1;
	} else if (bandera[37] >= 0 && (bandera[1] >= bandera[37])) {
		Emite(sistema[27]);			/* "No puedo llevar mas cosas" */
		return 1;
	} else {
		MueveObjeto(o, LLEVADO);	    /* "Llevado" */
		EmiteCadena(sistema[36], desc);    /* "Ahora tengo _" */
	}

	return 0;
}


int Dejar(int o)
{
	OBJETO *obj = DarObjeto(o);
	int loc = DarActualObjeto(obj);
	char *desc = DarDescripcionObjeto(obj);

	if (obj == NULL) {
		Emite(sistema[28]);				/* "No tengo eso" */
		return 1;
	} else if (loc == ENCIMA) {
		EmiteCadena(sistema[24], desc);	/* "No puedo. Llevo puesto _" */
		return 1;
	} else if (loc == bandera[38]) {
		EmiteCadena(sistema[49], desc);	/* "No tengo _" */
		return 1;
	} else if (loc != LLEVADO && loc != bandera[38]) {
		Emite(sistema[28]);			/* "No tengo eso" */
		return 1;
	} else {
		MueveObjeto(o, bandera[38]); /* Pasa a la localidad actual */
		EmiteCadena(sistema[39], desc);	  /* "He dejado caer _" */
	}

	return 0;
}


int Poner(int o)
{
	OBJETO *obj = DarObjeto(o);
	int loc = DarActualObjeto(obj);
	char *desc = DarDescripcionObjeto(obj);

	if (obj == NULL) {
		Emite(sistema[28]);					/* "No tengo uno de esos" */
		return 1;
	} else if (loc == bandera[38]) {
		EmiteCadena(sistema[49], desc);	/* "No tengo _" */
		return 1;
	} else if (loc == ENCIMA) {
		EmiteCadena(sistema[29], desc);	/* "Ya llevo puesto _" */
		return 1;
	} else if (loc != LLEVADO && loc != bandera[38]) {
		Emite(sistema[28]);			/* "No tengo uno de esos" */
		return 1;
	} else if (DarTipoObjeto(obj) != T_ROPA) {
		EmiteCadena(sistema[40], desc);	/* "No puedo ponerme _" */
		return 1;
	} else {
		MueveObjeto(o, ENCIMA);		/* Pasa a llevarse encima */
		EmiteCadena(sistema[37], desc);	/* "Ahora llevo puesto _" */
	}

	return 0;
}


int Quitar(int o)
{
	OBJETO *obj = DarObjeto(o);
	int loc = DarActualObjeto(obj);
	char *desc = DarDescripcionObjeto(obj);

	if (obj == NULL) {
		Emite(sistema[23]);			   /* "No llevo puesto uno de esos" */
		return 1;
	} else if (loc == bandera[38] || loc == LLEVADO) {
		EmiteCadena(sistema[50], desc);	/* "No llevo puesto _" */
		return 1;
	} else if (loc != bandera[38] && loc != ENCIMA) {
		Emite(sistema[23]);			/* "No llevo puesto uno de esos" */
		return 1;
	} else if (DarTipoObjeto(obj) != T_ROPA) {
		EmiteCadena(sistema[41], desc);	/* "No puedo quitarme _" */
		return 1;
	} else if (bandera[37] >= 0 && (bandera[1] >= bandera[37])) {
		EmiteCadena(sistema[42], desc);	/* "No puedo quitarme _. */
		return 1;						/*  Mis manos est n llenas" */
	} else {
		MueveObjeto(o, LLEVADO);		/* El objeto ahora se lleva */
		EmiteCadena(sistema[38], desc);	/* "Me he quitado _" */
	}

	return 0;
}


void Intercambiar(int o1, int o2)
{
	OBJETO *obj1 = DarObjeto(o1);
	OBJETO *obj2 = DarObjeto(o2);
	int locAux = DarActualObjeto(obj1);

	MueveObjeto(o1, DarActualObjeto(obj2));
	MueveObjeto(o2, locAux);
}


int Meter(int c, int o)
{
	OBJETO *obj = DarObjeto(o);
	char *desc = DarDescripcionObjeto(obj);
	int loc = DarActualObjeto(obj);
	int peso = DarPesoObjeto(obj);
	OBJETO *cont = DarObjeto(c);

	if (obj == NULL || cont == NULL) {
		Emite(sistema[28]);				/* "No tengo uno de esos" */
		return 1;
	} else if (DarTipoObjeto(cont) != T_CONTENEDOR) {
		EmiteLn("Argumento Ilegal: El objeto no es un contenedor");
		return 1;
	} else if (loc == ENCIMA) {
		EmiteCadena(sistema[24], desc);	/* "No puedo. Llevo puesto _" */
		return 1;
	} else if (loc == bandera[38]) {
		EmiteCadena(sistema[49], desc);	/* "No tengo _" */
		return 1;
	} else if (loc != LLEVADO && loc != bandera[38]) {
		Emite(sistema[28]);			/* "No tengo uno de esos" */
		return 1;
	} else {
		/* El objeto se mete dentro del contenedor */
		FijarPesoObjeto(cont, DarPesoObjeto(cont) + peso);
		MueveObjeto(o, DarAlmacenObjeto(cont));
		EmiteCadena(sistema[44], desc);	/* "El _ est  en " */
		Emite(DarDescripcionObjeto(cont));	/* Describe al contenedor */
		Emite(sistema[48]);			/* "." */
	}

	return 0;
}


int Sacar(int c, int o)
{
	OBJETO *obj = DarObjeto(o);
	char *desc = DarDescripcionObjeto(obj);
	int loc = DarActualObjeto(obj);
	int peso = DarPesoObjeto(obj);
	OBJETO *cont = DarObjeto(c);
	int almac = DarAlmacenObjeto(cont);

	if (obj == NULL || cont == NULL) {
		Emite(sistema[52]);					/* "No hay uno de esos en " */
		Emite(DarDescripcionObjeto(cont));
		Emite(sistema[48]);					/* "." */
		return 1;
	} else if (DarTipoObjeto(cont) != T_CONTENEDOR) {
		EmiteLn("Argumento Ilegal: El objeto no es un contenedor");
		return 1;
	} else if (loc == LLEVADO || loc == ENCIMA) {
		EmiteCadena(sistema[25], desc);	/* "Ya tengo _" */
		return 1;
	} else if (loc == bandera[38]) {
		EmiteCadena(sistema[45], desc);	/* "_ no est  en " */
		Emite(DarDescripcionObjeto(cont));
		Emite(sistema[48]);				/* "." */
		return 1;
	} else if (loc != bandera[38] && loc != almac) {
		Emite(sistema[52]);					/* "No hay uno de esos en " */
		Emite(DarDescripcionObjeto(cont));
		Emite(sistema[48]);				/* "." */
		return 1;
	} else if (loc != LLEVADO && loc != ENCIMA && bandera[52] >= 0 &&
			   (pesoTotal + DarPesoObjeto(obj) > bandera[52])) {
		EmiteCadena(sistema[46], desc);	/* "_ pesa mucho para m¡" */
		return 1;
	} else if (bandera[37] >= 0 && (bandera[1] >= bandera[37])) {
		Emite(sistema[27]);				/* "No puedo llevar m s cosas" */
		return 1;
	} else {
		FijarPesoObjeto(cont, DarPesoObjeto(cont) - peso);
		MueveObjeto(o, LLEVADO); 			/* Saco el objeto del cont. */
		EmiteCadena(sistema[36], desc);	/* "Ahora tengo _" */
	}

	return 0;
}


int SiguienteObjeto(int loc, int indic)
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

		sgteObjeto = inicial = SgteAux(localObj[loc]);

	} else if (sgteObjeto != NULL)
		sgteObjeto = sgteObjeto->siguiente;

	return (sgteObjeto == NULL) ? -1 : sgteObjeto->codigo;
}


static LOBJETO *SgteAux(LOBJETO *org)
{
	LOBJETO *dest;

	if (org == NULL)
		return NULL;
	else {
		dest = (LOBJETO*) Memoria((long) sizeof (LOBJETO));
		dest->codigo = org->codigo;
		dest->siguiente = SgteAux(org->siguiente);
		return dest;
	}
}

void ListaObj(int loc)
{
	ListaDeObjetos(loc, 0);
}


void ListaDe(int loc)
{
	ListaDeObjetos(loc, 1);
}


static void ListaDeObjetos(int loc, int sw)
{
	LOBJETO *l;
	int linea = bandera[53] == 0 || bandera[53] == 2;
	int cont = wherey();

	if (sw && localObj[loc] == NULL) {
		Emite(sistema[53]);		/* "Nada" */
		Emite(sistema[48]);		/* "." */
	} else {
		l = localObj[loc];

		if (l != NULL)

			if (linea) {

				if (!sw)
					Emite(sistema[1]);	/* "Ahora puedo ver..." */

				while (l != NULL) {

					if (l->siguiente == NULL) {
						Emite(DarDescripcionObjeto(DarObjeto(l->codigo)));
						Emite(sistema[48]);			/* "." */
						l = l->siguiente;
					} else if ((l->siguiente)->siguiente == NULL) {
						Emite(DarDescripcionObjeto(DarObjeto(l->codigo)));
						Emite(sistema[47]);			/* " y " */
						l = l->siguiente;
					} else {
						Emite(DarDescripcionObjeto(DarObjeto(l->codigo)));
						Emite(sistema[46]);			/* ", " */
						l = l->siguiente;
					}

				}

			} else {
				EmiteLn(sistema[1]);

				while (l != NULL) {
					if (++cont % 24 == 0) {
						Emite(sistema[7]);
						getch();
						gotoxy(1, wherey());
						clreol();
					}

					Emite(sistema[45]);

					if (l->siguiente == NULL)
						EmiteLn(DarDescripcionObjeto(DarObjeto(l->codigo)));
					else
						EmiteLn(DarDescripcionObjeto(DarObjeto(l->codigo)));

					l = l->siguiente;

				}

			}
	}

}


void Turnos(void)
{
	EmiteEntero(sistema[17], bandera[31]);
}


void Inventario(void)
{
	OBJETO *o;
	int loc = ENCIMA_LLEVADO;
	int linea = bandera[53] == 0 || bandera[53] == 1;
	LOBJETO *l = localObj[loc];
	int cont = wherey();

	if (linea)
		Emite(sistema[9]);				/* "Tienes " */
	else
		EmiteLn(sistema[9]);

	if (l == NULL)
		Emite(sistema[11]);			/* "Nada en absoluto" */
	else {
		l = localObj[loc];

		if (l != NULL)

			if (linea)

				while (l != NULL) {
					o = DarObjeto(l->codigo);

					if (l->siguiente == NULL) {
						Emite(DarDescripcionObjeto(o));

						if (DarActualObjeto(o) == ENCIMA)
							Emite(sistema[10]);			/* "(puesto)" */

						Emite(sistema[48]);			/* "." */
						l = l->siguiente;
					} else if ((l->siguiente)->siguiente == NULL) {
						Emite(DarDescripcionObjeto(o));

						if (DarActualObjeto(o) == ENCIMA)
							Emite(sistema[10]);			/* "(puesto)" */

						Emite(sistema[47]);				/* " y " */
						l = l->siguiente;
					} else {
						Emite(DarDescripcionObjeto(o));

						if (DarActualObjeto(o) == ENCIMA)
							Emite(sistema[10]);			/* "(puesto)" */

						Emite(sistema[46]);				/* ", " */
						l = l->siguiente;
					}
				}

			else

				while (l != NULL) {
					if (++cont % 24 == 0) {
						Emite(sistema[7]);
						getch();
						gotoxy(1, wherey());
						clreol();
					}

					o = DarObjeto(l->codigo);
					Emite(sistema[45]);

					if (l->siguiente == NULL || DarActualObjeto(o) == ENCIMA)
						Emite(DarDescripcionObjeto(o));
					else
						Emite(DarDescripcionObjeto(o));

					if (DarActualObjeto(o) == ENCIMA)
						EmiteLn(sistema[10]);			/* "(puesto)" */
					else
						EmiteLn("");

					l = l->siguiente;

				}

	}
}


void Tecla(void)
{
	Emite(sistema[16]);		/* "Pulsa una tecla para continuar..." */
	getch();
}


void Diagnostico(void)
{
	char buffer[8];
	int b, n, salir = 0;
	int anterior = bandera[11];

	bandera[11] = 0;

	while (!salir) {
		Emite("\n---------------------------------\nBandera n£mero "
			  "(ENTER para salir): ");
		Input(buffer, 7, 0);

		if (*buffer == '\0')
			salir = 1;
		else {
			b = atoi(buffer);

			if (b >= 0 && b < TAM_BANDERA) {
				EmiteEnteroLn("Valor actual: _", bandera[b]);
				Emite("Nuevo valor (ENTER para no cambiar): ");
				Input(buffer, 7, 0);
				bandera[b] = (*buffer == '\0') ? bandera[b] : atoi(buffer);
			} else
				EmiteEnteroLn("Bandera no existe; debe estar entre 0 y _",
							  TAM_BANDERA - 1);

		}

	}

	EmiteLn("---------------------------------");
	bandera[11] = anterior;
}


void SalvarSituacion(void)
{
	char fichero[MAXPATH];
	FILE *fp;
	int i, j, k = 0xF5;
	int anterior = bandera[11];
	char siono[4];
	struct ffblk ffblk;

	bandera[11] = 0;
	Emite(sistema[14]);
	Input(fichero, MAXPATH - 1, 0);
	bandera[11] = anterior;

	if (strlen(fichero) > 0) {
		if (findfirst(fichero, &ffblk, 0) == 0) {
			EmiteCadenaLn("ERROR: El fichero \"_\" ya existe. ¨Reemplazar?",
						  fichero);
			bandera[11] = 0;
			Emite("> ");
			Input(siono, 3, 0);
			bandera[11] = anterior;
			if (toupper(siono[0]) != sistema[30][0])
				return;
		}
		if ((fp = fopen(fichero, "wb")) == NULL) {
			EmiteCadenaLn("ERROR: El fichero \"_\" no puede crearse "
						  "(¨disco lleno?)", fichero);
			getch();
			return;
		}
	} else
		return;

	fwrite(&k, sizeof k, 1, fp);
	fwrite(bandera, sizeof bandera[0], TAM_BANDERA, fp);
	fwrite(&numObj, sizeof numObj, 1, fp);
	i = 0;

	while (i < numObj) {
		j = DarActualObjeto(indiceObj[i]);
		fwrite(&j, sizeof j, 1, fp);
		i++;
	}

	fclose(fp);
	EmiteLn("Ok");
}


int CargarSituacion(void)
{
	char fichero[MAXPATH];
	FILE *fp;
	int i, j, k;
	int anterior = bandera[11];

	bandera[11] = 0;
	Emite(sistema[15]);
	Input(fichero, MAXPATH - 1, 0);
	bandera[11] = anterior;

	if (strlen(fichero) > 0) {
		if ((fp = fopen(fichero, "rb")) == NULL) {
			EmiteCadenaLn("ERROR: fichero \"_\" no encontrado...", fichero);
			getch();
			return 0;
		}
	} else
		return 0;

	fread(&k, sizeof k, 1, fp);

	if (k != 0xF5) {
		EmiteCadenaLn("ERROR: fichero \"_\" con formato incorrecto...",
						fichero);
		getch();
		return 0;
	}

	fread(bandera, sizeof bandera[0], TAM_BANDERA, fp);
	fread(&numObj, sizeof numObj, 1, fp);
	i = 0;

	while (i < numObj) {
		fread(&j, sizeof j, 1, fp);
		MueveObjeto(i, j);
		i++;
	}

	fclose(fp);
	EmiteLn("Ok");
	getch();
	return 1;
}


void Salidas(void)
{
	CONEXION *c;
	char *dir;
	int linea = bandera[53] == 0;
	int oscuro, luzAusente;
	int loc = DarActualObjeto(DarObjeto(0));

	oscuro = bandera[0] != 0;
	luzAusente = loc != bandera[38] && loc != ENCIMA && loc != LLEVADO;


	if (oscuro && luzAusente)
		Emite(sistema[8]);		/* "No puedes hacer eso" */
	else if ((c = ConexionLocalidad(bandera[38])) == NULL)
		Emite(sistema[20]);		/* "No hay ninguna salida" */
	else {
		Emite(sistema[(c->siguiente == NULL) ? 19 : 18]);

		if (c != NULL)

			if (linea)

				while (c != NULL) {
					dir = DarLexemaPalabra(PrimeraVocab(c->direc));

					if (c->siguiente == NULL) {
						Emite(dir);
						Emite(sistema[48]);			/* "." */
						c = c->siguiente;
					} else if ((c->siguiente)->siguiente == NULL) {
						Emite(dir);
						Emite(sistema[47]);				/* " y " */
						c = c->siguiente;
					} else {
						Emite(dir);
						Emite(sistema[46]);				/* ", " */
						c = c->siguiente;
					}
				}

			else

				while (c != NULL) {
					dir = DarLexemaPalabra(PrimeraVocab(c->direc));
					Emite(sistema[45]);
					Emite(dir);
					c = c->siguiente;

					if (c == NULL)
						Emite(sistema[48]);		/* "." */
				}

	}

}


void Sonido(char *sonido, int loop)
{
	int i, j;

	if ((j = BuscaSonido(sonido)) == -1) {
		i = 0;

		while (sonidos[i] != NULL && sound_playing(i) && i < 8)
			i++;

		if (i >= 8)
			return;

		if (sonidos[i] != NULL && !sound_playing(i))
			BorrarSonido(i);

		if (!IncluirSonido(sonido, i))
			return;

		open_sound_resource_file("SONIDOS.SND");
		load_sound(&(sonidos[i]), sonido);
		close_sound_resource_file();
		start_sound(sonidos[i], i, loop);
	} else {
		stop_sound(j);
		start_sound(sonidos[j], j, loop);
	}

}


void Silencio(char *sonido)
{
	int j;

	if ((j = BuscaSonido(sonido)) != -1 && sound_playing(j)) {
		stop_sound(j);
		BorrarSonido(j);
	}

}


int BuscaSonido(char *clave)
{
	LISTASONIDOS *lista = listasonidos;

	while (lista != NULL) {
		if (strcmpi(lista->ident, clave) == 0)
			return lista->indice;

		lista = lista->siguiente;
	}

	return -1;
}


int IncluirSonido(char *p, int i)
{
	char *aux;
	int leng;
	LISTASONIDOS *son;

	if ((son = (LISTASONIDOS *) Memoria(sizeof (LISTASONIDOS))) == NULL)
		return 0;

	leng = strlen(p);
	aux = (char *) Memoria(leng + 1);
	strcpy(aux, p);
	aux[leng] = '\0';
	son->ident = aux;
	son->indice = i;
	son->siguiente = listasonidos;
	listasonidos = son;
	numSonidos++;

	return 1;
}


void BorrarSonido(int i)
{
	LISTASONIDOS *lista = listasonidos;
	LISTASONIDOS *p, *q;

	if (lista->indice == i) {
		p = lista->siguiente;
		free_sound(sonidos + i);
		sonidos[i] = NULL;
		farfree(lista);
		listasonidos = p;
		numSonidos--;
	} else {
		q = lista;
		lista = lista->siguiente;

		while (lista != NULL) {

			if (lista->indice == i) {
				p = lista->siguiente;
				q->siguiente = p;
				free_sound(sonidos + i);
				sonidos[i] = NULL;
				farfree(lista);
				numSonidos--;
				return;
			}

			q = lista;
			lista = lista->siguiente;
		}

	}

}


int Caminar(int loc, int direc)
{
	int salida = loc;
	CONEXION *co;

	co = ConexionLocalidad(loc);

	if (direc >= 0) {
		while (co != NULL && co->direc != direc)
			co = co->siguiente;

		if (co != NULL) { 					/* La conexion existe */
			salida = co->destino;
			bandera[51] = direc;
		}
	} else {								/* Aleatorio */
		while (salida == loc && co != NULL) {
			if (random(100) > 50) {
				salida = co->destino;
				bandera[51] = co->direc;
			} else
				co = co->siguiente;
		}

		if (co == NULL)
			bandera[51] = -1;
	}

	return salida;
}
