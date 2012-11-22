#include <string.h>
#include <alloc.h>
#include <stdio.h>
#include "T_Objeto.H"
#include "L_Objeto.H"
#include "Objeto.H"
#include "Lexico.H"						/* Para LONG_BUFFER */
#include "Local.H"						/* Para numLocal */
#include "Memoria.H"

OBJETO *tablaObj[TABLA_OBJETO];
OBJETO *indiceObj[INDICE_OBJETO];
int numObj = 0;

/* fHash():	Funci¢n de asociaci¢n de la tabla hash */
static int fHash(int nomb, int adj)
{
	return (nomb + adj) % TABLA_OBJETO;
}


/*
	InsertarObjeto():	Inserta un objeto en la tabla de objetos
	Recibe:    			Tipo, peso, nombre, adjetivo, descripci¢n,
						localidad inicial del objeto, la localidad almac‚n
						(usado s¢lo por los objetos contenedores) y c¢digo
						del objeto (si es -1, indica siguiente c¢digo libre
						al a¤adir)
	Devuelve:			1 si no hay errores; 0 en caso contrario
*/
int InsertarObjeto(TIPO_OBJETO tipoObj, int pe, int nomb, int adj,
				   char *desc, int inic, int act, int almac, int co)
{
	OBJETO *aux, *i;
	char *p;
	int pos, cod;
	int j;

	if (CodigoObjeto(nomb, adj) == -1) {	/* Si no existe ya ese objeto */

		if ((aux = (OBJETO*) Memoria((long) sizeof(OBJETO))) == NULL)
			return 0;

		if (co == -1) {
			for (j = 0; j < INDICE_OBJETO && indiceObj[j] != NULL; j++)
				;
			if (j >= INDICE_OBJETO)
				return 0;
			else
				cod = j;
		} else
			cod = co;

		aux->codigo = cod;
		aux->tipo = tipoObj;
		aux->peso = pe;
		aux->nombre = nomb;
		aux->adjetivo = adj;
		p = (char*) Memoria((long) strlen(desc) + 1);
		strcpy(p, desc);
		aux->descripcion = p;
		aux->inicial = inic;
		aux->actual = act;
		aux->almacen = almac;
		aux->siguiente = NULL;

		/* B£squeda por nombre y adjetivo */
		pos = fHash(nomb, adj);
		i = tablaObj[pos];

		if (i == NULL)
			tablaObj[pos] = aux;		 /* Actualizaci¢n del vector hash */
		else {

			while (i->siguiente != NULL)
				i = i->siguiente;

			i->siguiente = aux;
		}

		/* B£squeda por c¢digo */
		i = indiceObj[cod];

		if (i == NULL) {
			indiceObj[cod] = aux;			/* Actualizaci¢n del ¡ndice */
		}

		numObj++;
		return 1;
	} else
		return 0;
}


/*
	CodigoObjeto():	Determina el c¢digo de un cierto objeto dado su nombre
	Recibe:	    	Nombre y adjetivo del objeto
	Devuelve:		El c¢digo de dicho objeto, o -1 si no existe
*/
int CodigoObjeto(int nomb, int adj)
{
	OBJETO *aux;
	int encontrado;

	aux = tablaObj[fHash(nomb, adj)];
	encontrado = 0;

	while (aux != NULL && !encontrado) {

		if (aux->nombre == nomb && aux->adjetivo == adj)
			encontrado = 1;
		else
			aux = aux->siguiente;

	}

	return (encontrado) ? aux->codigo : -1;
}


/*
	DarObjetob():	Devuelve el objeto dado por su c¢digo
	Recibe:			El c¢digo del objeto
	Devuelve:		Un puntero al objeto
*/
OBJETO *DarObjeto(int cod)
{
	return (cod >= 0 && cod < numObj) ? indiceObj[cod] : NULL;
}


/*
	SalvarObjeto():	Salva la tabla de objetos en un stream
	Recibe:		   	El puntero al stream
*/
void SalvarObjeto(FILE *fp)
{
	int i, almac;
	int j = -1;
	int p;
	OBJETO *o;

	fwrite(&numObj, sizeof numObj, 1, fp);
	almac = 0;

	for (i = 0; i < numObj; i++) {
		o = indiceObj[i];

		while (o != NULL) {
			fwrite(&o->codigo, sizeof o->codigo, 1, fp);
			fwrite(&o->tipo, sizeof o->tipo, 1, fp);
			fwrite(&o->peso, sizeof o->peso, 1, fp);
			fwrite(&o->nombre, sizeof o->nombre, 1, fp);
			fwrite(&o->adjetivo, sizeof o->adjetivo, 1, fp);

			for (p = 0; p < strlen(o->descripcion); p++)
				(o->descripcion)[p] ^= 219;

			fwrite(o->descripcion, strlen(o->descripcion) + 1, 1, fp);
			fwrite(&o->inicial, sizeof o->inicial, 1, fp);

			/* Se le asigna un valor al atributo "almac‚n", en funci¢n
			   de las localidades libres que quedan */
			if (o->tipo == T_CONTENEDOR) {
				while (ExisteLocalidad(almac))
					almac++;
				fwrite(&almac, sizeof almac, 1, fp);
			} else
				fwrite(&j, sizeof j, 1, fp);

			o = o->siguiente;
		}
	}
}


/*
	CargarObjeto():	Carga la tabla de objetos desde un stream
	Recibe:		   	El puntero al stream
*/
void CargarObjeto(FILE *fp)
{
	int i, j, n, c;
	int p;
	char buffer[LONG_BUFFER];
	OBJETO o;

	fread(&n, sizeof n, 1, fp);			/* N§ de objetos almacenados */

	for (i = 0; i < n; i++) {
		fread(&o.codigo, sizeof o.codigo, 1, fp);
		fread(&o.tipo, sizeof o.tipo, 1, fp);
		fread(&o.peso, sizeof o.peso, 1, fp);
		fread(&o.nombre, sizeof o.nombre, 1, fp);
		fread(&o.adjetivo, sizeof o.adjetivo, 1, fp);
		j = 0;

		while ((c = getc(fp)) != '\0')
			buffer[j++] = c;

		buffer[j] = '\0';

		o.descripcion = (char*) Memoria((long) strlen(buffer) + 1);
		strcpy(o.descripcion, buffer);

		for (p = 0; p < strlen(o.descripcion); p++)
			o.descripcion[p] ^= 219;

		fread(&o.inicial, sizeof o.inicial, 1, fp);
		fread(&o.almacen, sizeof o.almacen, 1, fp);
		InsertarObjeto(o.tipo, o.peso, o.nombre, o.adjetivo, o.descripcion,
					   o.inicial, -1, o.almacen, o.codigo);

	}
}