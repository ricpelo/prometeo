#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>
#include <mem.h>
#include <time.h>
#include <dos.h>
#include "Bandera.H"
#include "Keys.H"
#include "Input.H"
#include "Runtime.H"
#include "Lexico.H"
#include "Alfa.H"

/* Declaraciones de variables globales y funciones */
static char *buffer;
int row;
int col;
int length;
int pos;
int tiempo;
int longTotal;

void BackSpace(void);
void Delete(void);
void DelWordLeft(void);
void DelWordRight(void);
void Display(void);
void End(void);
void Home(void);
void Insert(char ch);
void Left(void);
void OverStrike(char ch);
void Right(void);
void WordLeft(void);
void WordRight(void);


/* Borrar el car cter a la izquierda del actual */
static void BackSpace(void)
{
	if (pos > 0) {
		Left();
		Delete();
	}
}


/* Borrar car cter actual */
static void Delete(void)
{
	int cont;

	for (cont = pos; cont < length; cont++)
		buffer[cont] = buffer[cont + 1];

	buffer[length] = '\0';
	Display();
	longTotal = (longTotal > 0) ? longTotal - 1 : 0;
}


/* Borrar palabra a la izquierda de la posici¢n actual */
static void DelWordLeft(void)
{
	WordLeft();
	DelWordRight();
}


/* Borrar palabra a la derecha de la posici¢n actual */
static void DelWordRight(void)
{
	/* A¤adir validaci¢n para fin del buffer */
	while (!isspace(buffer[pos]))
		Delete();
}


/* Actualizar el b£ffer en pantalla */
static void Display(void)
{
	int fila, columna;
	int indic = 1;
	int cont;
	struct text_info ti;

	_setcursortype(_NOCURSOR);
	gettextinfo(&ti);
	fila = ti.curx; columna = ti.cury;
	gotoxy(col, row);

	for (cont = 0; cont <= length && buffer[cont]; cont++) {
		gettextinfo(&ti);
		if (ti.curx >= 79 && ti.cury >= ti.winbottom - ti.wintop + 1) {
			row--;
			indic = 0;
		}
		putch(buffer[cont]);
	}

	if (indic)
		putch(' ');

	gotoxy(fila, columna);
	_setcursortype(_SOLIDCURSOR);
}


/* Insertar un car cter en el b£ffer, desplazando caracteres a la dcha. */
static void Insert(char ch)
{
	int i;

	for (i = length; i > pos; i--)
		buffer[i] = buffer[i - 1];

	OverStrike(ch);
	longTotal++;
}


/* Moverse un car cter a la izquierda */
static void Left(void)
{
	struct text_info ti;

	if (pos > 0) {
		pos--;
		gettextinfo(&ti);
		if (ti.curx == 1)
			gotoxy(80, ti.cury - 1);
		else
			gotoxy(ti.curx - 1, ti.cury);
	}

}


/* Sobreescribir un car cter en la posici¢n actual */
static void OverStrike(char ch)
{
	struct text_info ti;
	buffer[pos] = ch;
	putch(ch);

	if (pos < length)
		pos++;
	else {
		gettextinfo(&ti);
		gotoxy(ti.curx - 1, ti.cury);
	}

}


/* Moverse un car cter a la derecha */
static void Right(void)
{
	struct text_info ti;

	gettextinfo(&ti);

	if (pos < length && buffer[pos] != '\0') {
		pos++;
		if (ti.curx >= 80) {
			if (ti.cury >= ti.winbottom - ti.wintop + 1)
				row--;
			gotoxy(1, ti.cury + 1);
		} else
			gotoxy(ti.curx + 1, ti.cury);
	}

}


/* Moverse una palabra a la izquierda */
static void WordLeft(void)
{
	Left();

	while (!isspace(buffer[pos]) && pos > 0)
		Left();

}


/* Moverse una palabra a la derecha */
static void WordRight(void)
{
	Right();

	while (!isspace(buffer[pos]) && buffer[pos + 1] != '\0' &&
		   pos < length)
		Right();

}


/* Entrada de datos en el b£ffer: es la funci¢n m s importante */
void Input(char *variable, int longitud, int lleno)
{
	int control;                       /* Indicador de tecla de control */
	char ch;
	time_t t;
	struct time hora;
	static char anterior[LONG_BUFFER];
	static int indic = 0;
	int i;
	int colIni;

	col = colIni = wherex();
	row = wherey();
	length = longitud;
	pos = longTotal = 0;
	memset(variable, '\0', longitud);

	if (!indic)
		memset(anterior, '\0', LONG_BUFFER);

	buffer = variable;

	do {
		t = time(NULL);

		while (!kbhit() && (bandera[11] > 0 &&
				(tiempo = time(NULL) - t > bandera[11]) == 0))
			;

		gettime(&hora);
		bandera[20] = hora.ti_hour;
		bandera[21] = hora.ti_min;
		bandera[22] = hora.ti_sec;
		bandera[23] = hora.ti_hund;

		if (tiempo && timeout != -1) {
/*			cprintf("\r\n"); */
			EmpujaPila(&direcciones, pc);
			pc = timeout;
			hecho = 0;					/* Para que el traductor no salga */
			Traductor();
			if (hecho) {
				memset(variable, '\0', longitud);
				return;
			} else
				continue;
		} else
			ch = getch();

		control = 0;

		if (ch == 0) {
			ch = getch();
			control = 1;
		}

		if (control) {

			switch (ch) {
				case K_F1:
					if (indic && anterior[pos] != '\0') {
						Insert(anterior[pos]);
						Display();
					}
					break;

				case K_F3:
					if (indic) {
						for (i = pos; i < strlen(anterior); i++) {
							Insert(anterior[pos]);
							Display();
						}
					}
					break;

				case K_LEFT:
					Left();
					break;

				case K_RIGHT:
					Right();
					break;

				case K_CTRL_LEFT:
					WordLeft();
					break;

				case K_CTRL_RIGHT:
					WordRight();
					break;

				case K_DELETE:
					Delete();
					break;

				case K_CTRL_T:
					DelWordRight();
					break;

				case '¤':
				case '¥':
					printf("¥");
					break;

			}

		} else {

			switch (ch) {
				case K_BS:               /* BS no es una tecla de control, */
					BackSpace();         /* es un car cter < 32 */
					break;

				case K_ENTER:
					for (i = 0; i < (longTotal - pos - colIni) / 80; i++)
						printf("\r\n");
					break;

				case '¤':
				case '¥':
					Insert('¥');
					Display();
					break;

				default:

					if (EsAlfanum(ch) || ch == ' ' || ch == '\"' ||
						ch == '.' || ch == ',' || ch == ';') {
						Insert(Mayus(ch));
						Display();
					}

			}

		}

	} while (ch != K_ENTER || (lleno && strlen(buffer) == 0));

	if (strlen(buffer) > 0) {
		strcpy(anterior, buffer);
		indic = 1;
	}

	if (!tiempo || timeout == -1)
		cprintf("\r\n");

}

