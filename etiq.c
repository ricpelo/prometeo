#include <string.h>
#include <alloc.h>
#include "Etiq.H"

int etiquetas[NUM_ETIQUETAS];

/*
	InsertarEtiqueta():	Inserta una etiqueta
	Recibe:    			El n£mero y la direcci¢n a la que hace referencia
*/
void InsertarEtiqueta(int num, int dir)
{
	if (num >= 0 && num < NUM_ETIQUETAS)
		etiquetas[num] = dir;
}


/*
	DireccionEtiqueta():   	Devuelve la direcci¢n que referencia la etiq.
	Recibe:    	    		El n£mero de la etiqueta
*/
int DireccionEtiqueta(int num)
{
	return (num >= 0 && num < NUM_ETIQUETAS) ? etiquetas[num] : -1;
}
