#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "mipwd.h"
#include "mirm.h"
#include "micp.h"
#include "mils.h"
#include "micat.h"
#include "mimkdir.h"
#include "mirmdir.h"

void ejecutar_orden (orden * O);

int main ()
{
    orden O;
    int r;

    printf ("Introduzca órdenes (pulse Ctrl-D para terminar)\n");

    do              // Leer órdenes y mostrarlas
    {
        inicializar_orden (&O);

        printf ("$ ");
        r = leer_orden (&O, stdin);

        if (r < 0)
            fprintf (stderr, "\nError %d: %s\n",
                             -r, mensajes_err[-r]);
        else
            ejecutar_orden (&O);

        liberar_orden (&O);
    }
    while (r==0);   // Repetir hasta error o EOF

    return 0;
}

void ejecutar_orden (orden * O)
{
    int i;

    printf ("\tOrden cruda: \"%s\"\n", O->orden_cruda);
    printf ("\tNúmero de argumentos: %d\n", O->argc);

    for (i=0; i<=O->argc; i++)
        if (O->argv[i] != NULL)
            printf ("\t\targv[%d]: \"%s\"\n", i, O->argv[i]);
        else
            printf ("\t\targv[%d]: NULL\n", i);

    if(strcmp(O->argv[0],"mipwd")==0)
    {
	mipwd();
    }

    if(strcmp(O->argv[0],"mils")==0)
    {
	mils(O->argv[1],O->argv[2]);
    }
    if(strcmp(O->argv[0],"mimkdir")==0)
    {
	mimkdir(O->argv[1]);
    }
    if(strcmp(O->argv[0],"mirmdir")==0)
    {
	mirmdir(O->argv[1]);
    }
    if(strcmp(O->argv[0],"micat")==0)
    {
	micat(O->argv[1]);
    }	
    if(strcmp(O->argv[0],"micp")==0)
    {
	micp(O->argv[1],O->argv[2]);
    }
    if(strcmp(O->argv[0],"mirm")==0)
    {	
	mirm(O->argv[1]);
    }
    if(strcmp(O->argv[0],"exit")==0)
    {
	exit(1);
    }
    if (O->entrada)
        printf ("\tEntrada: \"%s\"\n", O->entrada);

    if (O->salida)
        printf ("\tSalida: \"%s\"\n", O->salida);

    if (O->salida_err)
        printf ("\tSalida de err.: \"%s\"\n", O->salida_err);

    printf ("\tEjecutar en segundo plano: %s\n",
            O->segundo_plano ? "Sí" : "No");
}
