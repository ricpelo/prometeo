#include <alloc.h>
#include "L_Objeto.H"
#include "T_Objeto.H"
#include "Bandera.H"
#include "Local.H"
#include "Runtime.H"
#include "Memoria.H"

LOBJETO *localObj[NUM_LOCALIDADES];


void MueveObjeto(int cod, int local)
{
	OBJETO *obj = DarObjeto(cod);
	int locAux  = DarActualObjeto(obj);
	int banAux  = bandera[1];

	if (locAux == local)
		return;

	/* Decrementa b1 si el objeto estaba siendo llevado, y lo incrementa
	   si pasa ahora a ser llevado */

	bandera[1] = (local == LLEVADO) ? banAux + 1 :
				 (locAux == LLEVADO) ? banAux - 1 : banAux;

	if ((locAux == LLEVADO || locAux == ENCIMA) &&
		(local != LLEVADO && local != ENCIMA))
		pesoTotal -= DarPesoObjeto(obj);

	if ((locAux != LLEVADO && locAux != ENCIMA) &&
		(local == LLEVADO || local == ENCIMA))
		pesoTotal += DarPesoObjeto(obj);

	PasaObjetoLocal(cod, local);
	FijarActualObjeto(obj, local);
}


int MeteObjetoLocal(int cod, int local)
{
	LOBJETO *l, *aux;

	if (local < 0 || local > NUM_LOCALIDADES)
		return 0;

	if ((aux = (LOBJETO*) Memoria((long) sizeof (LOBJETO))) == NULL)
		return 0;

	aux->codigo = cod;
	aux->siguiente = NULL;

	l = localObj[local];

	if (l == NULL)
		localObj[local] = aux;
	else {

		while (l->siguiente != NULL)
			l = l->siguiente;

		l->siguiente = aux;
	}

	return 1;
}


void SacaObjetoLocal(int cod, int local)
{
	LOBJETO *p, *q;

	if (local >= 0 && local <= NUM_LOCALIDADES) {
		p = localObj[local];

		if (p->codigo == cod) {
			q = p->siguiente;
			farfree(p);
			localObj[local] = q;
		} else {

			while (p->siguiente != NULL && p->codigo != cod) {
				q = p;
				p = p->siguiente;
			}

			if (p->codigo == cod) {
				q->siguiente = p->siguiente;
				farfree(p);
			}

		}

	}

}


void PasaObjetoLocal(int cod, int local)
{
	OBJETO *obj = DarObjeto(cod);
	int actual = DarActualObjeto(obj);

	if (actual != local) {
		SacaObjetoLocal(cod, actual);
		MeteObjetoLocal(cod, local);

		if (local == ENCIMA || local == LLEVADO)
			MeteObjetoLocal(cod, ENCIMA_LLEVADO);

		if (actual == ENCIMA || actual == LLEVADO)
			SacaObjetoLocal(cod, ENCIMA_LLEVADO);

	}

}

