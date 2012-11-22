#include <stdio.h>
#include <stdlib.h>
#include <dir.h>
#include <string.h>
#include <ctype.h>
#include "Tabla.H"
#include "yytabmp.h"

#define LONG_CAD        2048

FILE *yyin;
FILE *yyout;
char yytext[LONG_CAD];
void yyparse(void);
void CompilacionSeparada(FILE *lista);
void CompilarModulo(char *fich);
int linea = 1;
int loc = 0;


int EsAlfa(int c)
{
	return isalpha(c) || c == '†' || c == 'Ç' || c == '°' || c == '¢' ||
						 c == '£' || c == 'ê';
}


int EsAlfanum(int c)
{
	return EsAlfa(c) || isdigit(c) || c == '_';
}


void yylex(void)
{
	int c;
	int pos;
	char ident[LONG_CAD];
	char *i;

	while (1) {
		memset(yytext, 0, sizeof(yytext));
		c = getc(yyin);

		while (isspace(c)) {
			if (c == '\n')  linea++;
			putc(c, yyout);
			c = getc(yyin);
		}

		pos = 0;

		if (c == '\"') {
			putc(c, yyout);
			c = getc(yyin);
			while (c != '\"') {
				if (c == '\\') {
					c = getc(yyin);
					if (c == '\\') {
						c = getc(yyin);
						while (c != '\n' && c != EOF)
							c = getc(yyin);
						putc(c, yyout);
						if (c == '\n')  linea++;
					} else if (c == '\"') {
						putc('\\', yyout);
						putc('\"', yyout);
					} else {
						putc('\\', yyout);
						putc(c, yyout);
						if (c == '\n')  linea++;
					}
				} else {
					putc(c, yyout);
					if (c == '\n')  linea++;
				}
				c = getc(yyin);
			}
			putc(c, yyout);
		} else if (EsAlfanum(c) || c == '#') {
			yytext[pos++] = c;
			c = getc(yyin);

			while (EsAlfanum(c)) {
				yytext[pos++] = c;
				c = getc(yyin);
			}

			ungetc(c, yyin);

			if (strcmp(yytext, "#define") == 0) {
				memset(yytext, 0, sizeof(yytext));
				memset(ident, 0, sizeof(ident));
				c = getc(yyin);

				while (isspace(c)) {
					if (c == '\n')  linea++;
					putc(c, yyout);
					c = getc(yyin);
				}

				pos = 0;

				while (EsAlfanum(c)) {
					ident[pos++] = c;
					c = getc(yyin);
				}

				ungetc(c, yyin);
				if ((i = DarInfo(ident)) != NULL) {
					printf("ERROR (linea %d): Macro ya declarada\n", linea);
					exit(1);
				}
				memset(yytext, 0, sizeof(yytext));
				c = getc(yyin);

				while (isspace(c)) {
					if (c == '\n')  linea++;
					putc(c, yyout);
					c = getc(yyin);
				}

				pos = 0;

				while (c != '\n' && c != EOF) {
					yytext[pos++] = c;
					c = getc(yyin);
				}

				ungetc(c, yyin);
				InsertarElemento(ident, yytext);
			} else if (strcmp(yytext, "#local") == 0) {
				yytext[0] = '\0';

				while (strcmp(yytext, "#finlocal") != 0 && c != EOF) {
					c = getc(yyin);

					while (isspace(c)) {
						if (c == '\n')  linea++;
						putc(c, yyout);
						c = getc(yyin);
					}

					pos = 0;
					memset(yytext, 0, sizeof(yytext));

					while (EsAlfanum(c) || c == '#') {
						yytext[pos++] = c;
						c = getc(yyin);
					}

					ungetc(c, yyin);

					if (strcmp(yytext, "#finlocal") != 0)
						if ((i = DarInfo(yytext)) != NULL) {
							printf("ERROR (linea %d): Macro ya declarada\n",
								   linea);
							exit(1);
						} else
							InsertarElemento(yytext, itoa(loc++, ident, 10));
				}
			} else {
				if ((i = DarInfo(yytext)) != NULL)
					fprintf(yyout, "%s", i);
				else
					fprintf(yyout, "%s", yytext);
			}
		} else if (c == EOF)
			return;
		else
			putc(c, yyout);

	}

}


int main(int argc, char *argv[])
{
	char fich[MAXPATH],
		 drive[MAXDRIVE],
		 dir[MAXDIR],
		 file[MAXFILE],
		 ext[MAXEXT];
	int flag;
	FILE *fp1, *fp2;
	FILE *lista;
	struct ffblk fb;

	printf("PROMETEO 2.2 - Generador de Aventuras Conversacionales\n");
	printf("Preprocesador (c) 1995-97 Ricardo PÇrez L¢pez - Alpha Aventuras\n");

	if (argc == 1)

		if (findfirst("*.PRO", &fb, 0) == -1) {
			printf("\tSintaxis: PP [ fichero[.PRO] | @fichero[.PL] ]\n");
			exit(1);
		} else {
			printf("(usar opci¢n /h o /? para instrucciones de uso)\n\n");
			strcpy(fich, fb.ff_name);
			CompilarModulo(fich);
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
					printf("\n\tSintaxis: PP [ fichero[.PRO] | "
							"@fichero[.PL] ]\n");
					exit(1);
					break;

				default:
					printf("\n\tOpci¢n desconocida\n");
					exit(1);

			}

		} else {
			printf("(usar opci¢n /h o /? para instrucciones de uso)\n\n");
			strcpy(fich, argv[1]);
			CompilarModulo(fich);
		}

	}

	return 0;
}


void CompilacionSeparada(FILE *lista)
{
	char fich[MAXPATH];
	char *p;

	p = fgets(fich, MAXPATH + 1, lista);

	if (p == NULL) {
		printf("No hay m¢dulos que compilar\n");
		exit(1);
	}

	while (p != NULL) {
		*(strchr(fich, '\n')) = '\0';
		if (fich[0] != '\0')
			CompilarModulo(fich);
		p = fgets(fich, MAXPATH + 1, lista);
	}

}


void CompilarModulo(char *fich)
{
	int flag;
	char drive[MAXDRIVE],
		 dir[MAXDIR],
		 file[MAXFILE],
		 ext[MAXEXT];
	FILE *fp1, *fp2;

	flag = fnsplit(fich, drive, dir, file, ext);

	if (!(flag & EXTENSION)) {
		strcpy(ext, ".PRO");
		fnmerge(fich, drive, dir, file, ext);
	}

	if ((fp1 = fopen(fich, "rt")) == NULL) {
		printf("El fichero \"%s\" no existe\n", fich);
		exit(1);
	}

	yyin = fp1;

	printf("Preprocesando \"%s\"...\n", fich);
	flag = fnsplit(fich, drive, dir, file, ext);
	strcpy(ext, ".PPP");
	fnmerge(fich, drive, dir, file, ext);

	if ((fp2 = fopen(fich, "wb")) == NULL) {
		printf("ERROR: El fichero \"%s\" no puede abrirse para escritura\n",
				fich);
		exit(1);
	}

	yyout = fp2;

	yylex();
	fclose(fp2);
	fclose(fp1);
	printf("Fichero preprocesado en \"%s\"\n", fich);
}
