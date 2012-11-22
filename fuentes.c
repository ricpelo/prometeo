#include <dos.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include "Acciones.H"
#include "Fuentes.H"

typedef unsigned int word;
typedef unsigned char byte;

static byte pbyNew[4096];
static int ldFont = 0;

void SetFont(void)
{
	byte far *pby = pbyNew;

	if (ldFont)
		asm {
			push bp
			mov ah, 0x11
			mov al, 0
			mov bh, 16                 /* 16 bytes por car cter */
			mov bl, 0
			mov cx, 255                /* antes pon¡a 97 caracteres */
			mov dl, 0                  /* antes pon¡a '!' */
			mov dh, 0
			les bp, pby
			int 0x10
			pop bp
		}

}


void LoadFileFont(char *fich)
{
	int i;
	FILE *fp;

	if ((fp = fopen(fich, "rb")) == NULL) {
		EmiteCadenaError("ERROR: No se puede cargar la fuente _", fich);
		return;
	}

	fread(pbyNew, sizeof(byte), 4096, fp);
	ldFont = 1;
	SetFont();
}
