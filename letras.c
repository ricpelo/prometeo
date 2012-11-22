#include <dos.h>
#include <conio.h>
#include <stdio.h>
#include "Letras.H"

typedef unsigned int word;
typedef unsigned char byte;


byte far *LoadFont(void);
void SetFont(byte far *pby);

byte far *LoadFont(void)
{
	word wSegment, wOffset;

	asm {
		mov di, bp
		mov ah, 0x11
		mov al, 0x30
		mov bh, 6
		int 0x10
		push es
		push bp
		mov bp, di
		pop ax
		mov wOffset, ax
		pop ax
		mov wSegment, ax
	}

	return (byte far *)(MK_FP(wSegment, wOffset));
}

void SetFont(byte far *pby)
{
	asm {
		push bp
		mov ah, 0x11
		mov al, 0
		mov bh, 16                      /* 16 bytes por car cter */
		mov bl, 0
		mov cx, 222                     /* 97 caracteres */
		mov dl, '!'                     /* car cter a partir del cual comienzo */
		mov dh, 0
		les bp, pby
		int 0x10
		pop bp
	}

}

void Bold(void)
{
	byte far *pbyOld, pbyNew[16 * 222];
	int i;

	pbyOld = LoadFont();

	for (i = 0; i < 16 * 222; i++)
		pbyNew[i] = (pbyOld[i + 16 * 33] >> 1) | pbyOld[i + 16 * 33];

	SetFont(pbyNew);
}

void Underlined(void)
{
	byte far *pbyOld, pbyNew[16 * 222];
	int i;

	pbyOld = LoadFont();

	for (i = 0; i < 16 * 222; i++)

		if (i % 16 == 15)
			pbyNew[i] = 0xFF;
		else
			pbyNew[i] = pbyOld[i + 16 * 33];

	SetFont(pbyNew);
}

void Cursive(void)
{
	byte far *pbyOld, pbyNew[16 * 222];
	int i;                          

	pbyOld = LoadFont();

	for (i = 0; i < 16 * 222; i++)

		switch(i % 16) {
			case 2:
			case 3:
				pbyNew[i] = pbyOld[i + 16 * 33] >> 2;
				break;

			case 4:
			case 5:
				pbyNew[i] = pbyOld[i + 16 * 33] >> 1;
				break;

			case 10:
			case 11:
				pbyNew[i] = pbyOld[i + 16 * 33] << 1;
				break;

			case 12:
			case 13:
				pbyNew[i] = pbyOld[i + 16 * 33] << 2;
				break;

			default:
				pbyNew[i] = pbyOld[i + 16 * 33];

		}

	SetFont(pbyNew);
}
