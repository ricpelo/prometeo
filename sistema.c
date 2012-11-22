#include <string.h>
#include <alloc.h>
#include <stdio.h>
#include "Sistema.H"
#include "Lexico.H"						/* Para LONG_BUFFER */
#include "Memoria.H"

char *sistema[NUM_SISTEMA];
int numSist = 0;

char *inic[] = {
		"Esta muy oscuro para poder ver nada.",
		" Tambi‚n veo ",
		"\nDi algo coherente.",
		"\n­Vamos! ¨A qu‚ esperas?",
		"\n¨Qu‚ quieres que haga?",
		"\nDime que hacemos...",
		"No entiendo lo que quieres decirme.",
		"<<< MAS >>>",
		"No puedo hacer eso.",
		"Llevas ",
		" (encima)",
		"nada en absoluto.",
		"¨Estas seguro?",
		"\n¨Quieres jugar otra vez?",
		"Nombre del fichero a salvar: ",
		"Nombre del fichero a recuperar: ",
		"\nPulsa una tecla para continuar...\n",
		"Has hecho _ acciones(s) v lida(s).\n",
		"Las salidas posibles son ",
		"Unicamente ",
		"No hay ninguna salida.",
		"No hay salida en esa direcci¢n.",
		"",
		"No llevo puesto uno de esos.",
		"No puedo. Ya llevo puesto _.",
		"Ya tengo _.",
		"No hay uno de esos por aqu¡.",
		"Ya no puedo llevar m s cosas.",
		"No tengo eso.",
		"Ya llevo puesto _.",
		"S",
		"S",
		 "",
		"\n>>> ",
		"",
		"",
		"Ahora tengo _.",
		"Ahora llevo puesto _.",
		"Me he quitado _.",
		"He dejado caer _.",
		"No puedo ponerme _.",
		"No me puedo quitar _.",
		"No puedo quitarme _. Mis manos est n llenas.",
		"_ pesa mucho para m¡.",
		"_ est  en ",
		"- ",
		", ",
		" y ",
		".",
		"No tengo _.",
		"No llevo puesto _.",
		"",
		"No hay uno de esos en ",
		"nada de nada" };


/*
	CambiarSistema():  	Cambia un mensaje del sistema
	Recibe:    			El n£mero y la descripci¢n
	Devuelve:			1 si se ha cambiado con ‚xito. 0 en caso contrario
*/
int CambiarSistema(int num, char *desc)
{
	char *p;

	if (num >= 0 && num < NUM_SISTEMA) {
		if ((p = (char*) Memoria((long) strlen(desc) + 1)) == NULL)
			return 0;

		strcpy(p, desc);
		sistema[num] = p;
		numSist++;
		return 1;
	} else
		return 0;
}


/*
	DescripcionSistema():	Devuelve un puntero a la descripci¢n del mensaje
	Recibe:    	    		El n£mero del mensaje
*/
char *DescripcionSistema(int num)
{
	return (num >= 0 && num < NUM_SISTEMA) ? sistema[num] : NULL;
}


/*
	SalvarSistema():	Salva la tabla de mensajes del sistema en un stream
	Recibe:				El puntero al stream
*/
void SalvarSistema(FILE *fp)
{
	int i, j;
	char *s;

	fwrite(&numSist, sizeof numSist, 1, fp);

	for (i = 0; i < NUM_SISTEMA; i++) {
		s = sistema[i];

		if (s != NULL) {
			fwrite(&i, sizeof i, 1, fp);

			for (j = 0; j < strlen(s); j++)
				s[j] ^= 219;

			fwrite(s, strlen(s) + 1, 1, fp);
		}
	}
}


/*
	CargarSistema():	Carga tabla de mensajes del sistema desde un stream
	Recibe:				Un puntero al stream
*/
void CargarSistema(FILE *fp)
{
	int i, j, k, n;
	int p;
	int c;
	char *s, buffer[LONG_BUFFER];

	for (i = 0; i <= 53; i++)
		sistema[i] = inic[i];

	fread(&n, sizeof n, 1, fp);	/* N§ de mensajes del sistema almacenados */

	for (i = 0; i < n; i++) {
		fread(&k, sizeof i, 1, fp);
		j = 0;
		while ((c = getc(fp)) != '\0')
			buffer[j++] = c;

		buffer[j] = '\0';
		s = (char*) Memoria((long) strlen(buffer) + 1);
		strcpy(s, buffer);

		for (p = 0; p < strlen(s); p++)
			s[p] ^= 219;

		CambiarSistema(k, s);
	}
}
