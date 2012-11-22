#include <string.h>
#include <alloc.h>
#include <stdio.h>
#include "T_Mens.H"				/* Para guardar las im genes */
#include "Lexico.H"				/* Para LONG_BUFFER */
#include "Local.H"
#include "Memoria.H"

static CONEXION *ConexAux(FILE *fp);

LOCAL local[NUM_LOCALIDADES];
int numLocal = 0;


/*
	InsertarLocalidad():	Inserta una localidad
	Recibe:    				El n£mero, la descripci¢n y un puntero a sus
							conexiones
	Devuelve:				1 si se ha insertado con ‚xito. 0 en otro caso
*/
int InsertarLocalidad(int num, char *desc, int imagen, CONEXION *conex)
{
	char *p;

	if (num >= 0 && num < NUM_LOCALIDADES - 3) {
		if ((p = (char*) Memoria((long) strlen(desc) + 1)) == NULL)
			return 0;

		strcpy(p, desc);
		local[num].descripcion = p;
		local[num].pantalla = imagen;
		local[num].conexion = conex;
		numLocal++;
		return 1;
	} else
		return 0;
}


/*
	DescripcionLocalidad():	Devuelve un puntero a la descripci¢n de la loc.
	Recibe:    	    		El n£mero de la localidad
*/
char *DescripcionLocalidad(int num)
{
	return (num >= 0 && num < NUM_LOCALIDADES) ?
			local[num].descripcion : NULL;
}


/*
	PantallaLocalidad():	Devuelve un puntero al nombre de fichero de la
							pantalla de una cierta localidad
	Recibe:    	    		El n£mero de la localidad
*/
int PantallaLocalidad(int num)
{
	return (num >= 0 && num < NUM_LOCALIDADES) ? local[num].pantalla :
			NULL;
}


/*
	ConexionLocalidad():	Devuelve un puntero a las conexiones de la loc.
	Recibe:					El n£mero de la localidad
*/

CONEXION *ConexionLocalidad(int num)
{
	return (num >= 0 && num < NUM_LOCALIDADES) ?
			local[num].conexion : NULL;
}


/*
	ExisteLocalidad():	Determina si una cierta localidad ha sido creada
	Recibe:				El n£mero de la localidad
	Devuelve:			1 si est  creada; 0 en caso contrario
*/
int ExisteLocalidad(int num)
{
	/* Las £ltimas tres localidades (N_L - 3 , N_L - 2 y N_L - 1) siempre
	   se considera que existen, porque representan los objetos que no se
	   han creado, los que se llevan puestos encima y los que se llevan
	   en el inventario, respectivamente */
	return DescripcionLocalidad(num) != NULL ||
		   (num >= NUM_LOCALIDADES - 3 && num < NUM_LOCALIDADES);
}


/*
	SalvarLocalidad():	Salva la tabla de localidades en un stream
	Recibe:				El stream
*/
void SalvarLocalidad(FILE *fp)
{
	int i, j = -1;
	int k, c, p;
	LOCAL l;
	CONEXION *co;

	fwrite(&numLocal, sizeof numLocal, 1, fp);

	for (i = 0; i < NUM_LOCALIDADES; i++) {
		l = local[i];

		if (l.descripcion != NULL) {
			fwrite(&i, sizeof i, 1, fp);

			for (p = 0; p < strlen(l.descripcion); p++)
				l.descripcion[p] ^= 219;

			fwrite(l.descripcion, strlen(l.descripcion) + 1, 1, fp);
			fwrite(&l.pantalla, sizeof l.pantalla, 1, fp);

			co = l.conexion;

			while (co != NULL) {
				fwrite(&co->direc, sizeof co->direc, 1, fp);
				fwrite(&co->destino, sizeof co->destino, 1, fp);
				co = co->siguiente;
			}

			fwrite(&j, sizeof(int), 1, fp);	/* -1 = No hay m s conexiones */
		}
	}
}


/*
	CargarLocalidad():	Carga la tabla de localidades desde un stream
	Recibe:				El stream
*/
void CargarLocalidad(FILE *fp)
{
	int i, j, k, c, n;
	int p;
	char buffer[LONG_BUFFER];
	LOCAL l;
	CONEXION *co;

	fread(&n, sizeof n, 1, fp);			/* N§ de localidades almacenadas */

	for (k = 0; k < n; k++) {
		fread(&i, sizeof i, 1, fp);
		j = 0;

		while ((c = getc(fp)) != '\0')
			buffer[j++] = c;

		buffer[j] = '\0';
		l.descripcion = (char*) Memoria((long) strlen(buffer) + 1);
		strcpy(l.descripcion, buffer);

		for (p = 0; p < strlen(l.descripcion); p++)
			l.descripcion[p] ^= 219;

		fread(&l.pantalla, sizeof l.pantalla, 1, fp);
		l.conexion = ConexAux(fp);
		InsertarLocalidad(i, l.descripcion, l.pantalla, l.conexion);
	}
}


/*
	ConexAux():	Funci¢n auxiliar para recuperar las conexiones de una cierta
				localidad
	Recibe:		Un puntero al stream de donde leer
	Devuelve:	El puntero a la lista de conexiones de la localidad
	Notas:		Es una funci¢n recursiva
*/
static CONEXION *ConexAux(FILE *fp)
{
	int c;
	CONEXION *co;

	fread(&c, sizeof c, 1, fp);

	if (c == -1)
		return NULL;
	else {
		co = (CONEXION*) Memoria((long) sizeof(CONEXION));
		co->direc = c;
		fread(&co->destino, sizeof co->destino, 1, fp);
		co->siguiente = ConexAux(fp);
		return co;
	}

}
