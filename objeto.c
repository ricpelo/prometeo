#include "Objeto.H"


/*	DarCodigoObjeto():	Devuelve el c¢digo de un cierto objeto */
int DarCodigoObjeto(OBJETO *objeto)
{
	return (objeto != NULL) ? objeto->codigo : -1;
}


/*
	FijarCodigoObjeto():	Fija el c¢digo de un objeto
	Recibe:					El c¢digo
*/
void FijarCodigoObjeto(OBJETO *objeto, int cod)
{
	objeto->codigo = cod;
}


/* DarTipoObjeto():		Devuelve el tipo del objeto */
TIPO_OBJETO DarTipoObjeto(OBJETO *objeto)
{
	return (objeto != NULL) ? objeto->tipo : -1;
}


/*
	FijarTipoObjeto():	Fija el tipo de un objeto
	Recibe:				El tipo
*/
void FijarTipoObjeto(OBJETO *objeto, TIPO_OBJETO t)
{
	objeto->tipo = t;
}


/* DarPesoObjeto():	Devuelve el peso del objeto */
int DarPesoObjeto(OBJETO *objeto)
{
	return (objeto != NULL) ? objeto->peso : -1;
}


/*
	FijarPesoObjeto(): 	Fija el peso de un objeto
	Recibe:				El peso
*/
void FijarPesoObjeto(OBJETO *objeto, int p)
{
	objeto->peso = p;
}


/* DarNombreObjeto():	Determina el nombre de un objeto */
void DarNombreObjeto(OBJETO *objeto, int *nomb, int *adj)
{
	*nomb = objeto->nombre;
	*adj  = objeto->adjetivo;
}


/*
	FijarNombreObjeto():	Fija el nombre de un objeto
	Recibe:					El nombre y otro al adjetivo
*/
void FijarNombreObjeto(OBJETO *objeto, int nomb, int adj)
{
	objeto->nombre   = nomb;
	objeto->adjetivo = adj;
}


/*
	DarDescripcionObjeto():	Devuelve un puntero a la descripci¢n del
							objeto
	Recibe:					El objeto
	Devuelve:				Un puntero a car cter (la descripci¢n)
*/
char *DarDescripcionObjeto(OBJETO *objeto)
{
	return (objeto != NULL) ? objeto->descripcion : NULL;
}


/*
	DarInicialObjeto():	Devuelve la localidad inicial de un objeto
	Recibe:				El objeto
*/
int DarInicialObjeto(OBJETO *objeto)
{
	return (objeto != NULL) ? objeto->inicial : -1;
}


/*
	FijarInicialObjeto():	Fija la localidad inicial de un objeto
	Recibe:					Un puntero al objeto, y el n£m. de localidad
*/
void FijarInicialObjeto(OBJETO *objeto, int inic)
{
	objeto->inicial = inic;
}


/*
	DarActualObjeto():	Devuelve la localidad actual de un objeto
	Recibe:				El objeto
*/
int DarActualObjeto(OBJETO *objeto)
{
	return (objeto != NULL) ? objeto->actual : -1;
}


/*
	FijarActualObjeto():	Fija la localidad actual de un objeto
	Recibe:					Un puntero al objeto, y el n£m. de localidad
*/
void FijarActualObjeto(OBJETO *objeto, int act)
{
	objeto->actual = act;
}


/* DarAlmacenObjeto():	Determina la localidad almac‚n de un contenedor */
int DarAlmacenObjeto(OBJETO *objeto)
{
	return (objeto != NULL) ? objeto->almacen : -1;
}


/*
	FijarAlmacenObjeto():	Fija la localidad almacen de un contenedor
	Recibe:					Un puntero al objeto, y el n£m. de localidad
*/
void FijarAlmacenObjeto(OBJETO *objeto, int almac)
{
	objeto->almacen = almac;
}


/* DarSiguienteObjeto():	Determina siguiente objeto en tabla Hash */
OBJETO *DarSiguienteObjeto(OBJETO *objeto)
{
	return (objeto != NULL) ? objeto->siguiente : NULL;
}


/*
	FijarSiguienteObjeto(): 	Fija siguiente objeto en tabla Hash
	Recibe:				   		Un puntero a objeto
*/
void FijarSiguienteObjeto(OBJETO *o1, OBJETO *o2)
{
	o1->siguiente = o2;
}
