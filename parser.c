/*
    parser.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include "parser.h"

// Array que contiene los posibles mensajes de error (ligado a
// las definiciones hechas en parser.h):

const char * const mensajes_err[] =
{
    "Todo bien",                //                          0
    "Fin de fichero",           // ERROR_EOF               -1
    "Parámetros incorrectos",   // ERROR_PARAMETROS_MAL    -2
    "No hay suficiente memoria" // ERROR_FALTA_MEMORIA     -3
};

// Declaración de funciones privadas de este módulo (no se
// pueden invocar directamente desde otros módulos):

static int leer_linea (char ** plinea, FILE * stream);

/*  --------------------------------------------------------------
    inicializar_orden ()

    "Resetear" una estructura para poder pasársela después
    a leer_orden().
*/

void inicializar_orden (orden * O)
{
    if (O)
        memset (O, 0, sizeof(orden));
}

/*  --------------------------------------------------------------
    leer_orden ()

    Leer e interpretar ("parsear") una orden tomada de un archivo
    o del teclado, separando los distintos argumentos, aplicando
    caracteres de escape y comillas etc.

    Parámetros:

        O           Estructura en la que almacenar la orden
                    interpretada.

        stream      Archivo o 'flujo' del que leer la orden (si
                    es NULL, se usará la entrada estándar).

    Resultado:

        Se devolverá 0 si todo va bien, o un valor negativo si
        se produce algún error. En caso de error, se puede
        obtener el mensaje de error cambiando de signo al valor
        devuelto y utilizándolo como índice en el array
        mensajes_err.

    Antes de usar esta función hay que llamar a la función
    inicializar_orden(). De lo contrario, leer_orden()
    devolverá el código de error ERROR_PARAMETROS_MAL.

    Tras llamar a leer_orden(), una vez que se haya terminado de
    trabajar con la orden interpretada, hay que llamar a la
    función liberar_orden() para que libere la memoria dinámica
    que reservó leer_orden().
*/

int leer_orden (orden * O, FILE * stream)
{
    int n;
    char ** argv;       // Vector de argumentos
    int argc, argr;     // Tamaño usado/reservado de argv
    char * p;           // Pos. actual en buffer de origen
    char * q;           // Pos. actual en buffer de destino
    char * trozo;       // "Palabra" actual en buff. destino
    char comillas;      // '\'' o '\"' si hay comillas
    char escape;        // '\\' si hay secuencia de escape
    char redireccion;   // 1=="<..."  2==">..."  3=="2>..."

    if (O == NULL || O->orden_cruda != NULL ||
                     O->buffer_trozos != NULL ||
                     O->argv != NULL)
        return ERROR_PARAMETROS_MAL;

    n = leer_linea (&O->orden_cruda, stream);

    if (n <= 0)     // Si error u orden vacía,
        return n;   // se acabó

    // Saltar espacio en blanco inicial

    for (p=O->orden_cruda; *p && strchr(" \t\n",*p); p++)
    {}

    if (*p == '\0')    // Si no queda nada, no hay orden
        return 0;      // ni parámetros

    // Reservar buffer de trozos de orden (no se inicializa aquí
    // porque se irá copiando la orden sobre la marcha)

    O->buffer_trozos = (char*) malloc ((1+strlen(p))*
                                       sizeof(char));

    if (O->buffer_trozos == NULL)
        return ERROR_FALTA_MEMORIA;

    // Reservar array de punteros a argumentos (en principio unos
    // pocos; luego se irá dulicando el tamaño si hace falta)

    argr = 4;
    O->argc = argc = 0;
    O->argv = argv = (char**) malloc (argr*sizeof(char*));

    if (argv == NULL)
        return ERROR_FALTA_MEMORIA;

    // Recorrer el buffer carácter a carácter, interpretando
    // las distintas partes de la orden.

    // La orden se irá copiando de O->orden_cruda a
    // O->buffer_trozos con operaciones como:  *q++ = *p++;

    // Pero además se irán interpretando las secuencias de escape,
    // las comillas, las redirecciones y el & (ejecutar en
    // segundo plano, o 'backgroud').

    // Nótese que O->buffer_trozos contendrá más de un '\0' de
    // fin de cadena, porque alojará varias cadenas (todos los
    // argumentos y las redirecciones). Los elementos del
    // array argv y los punteros de las redirecciones apuntarán
    // al interior de este buffer.

    O->entrada = O->salida = O->salida_err = NULL;
    comillas = escape = redireccion = O->segundo_plano = 0;

    p = O->orden_cruda;
    trozo = q = O->buffer_trozos;

    for (;;)
    {
        if (!escape && comillas!='\'' && *p=='\\')
        {
            escape = *p++;        // Carácter de escape...
            continue;             // "escapar" el siguiente car.
        }

        if (!escape && !comillas && (*p=='\'' || *p=='\"'))
        {
            comillas = *p++;      // Apertura de comillas...
            continue;             // levantar bandera de comillas
        }

        if (!escape && comillas && *p==comillas)
        {
            comillas = 0;         // Cierre de comillas actuales..
            p ++;                 // bajar bandera de comillas
            continue;
        }

        if (*p && (escape || comillas ||
                   !strchr(" \t\n<>&",*p)))
        {
            escape = 0;           // Carácter normal (o escapado
            *q++ = *p++;          // por \ ó comillas)...
            continue;             // copiar y avanzar
        }
                           // Carácter especial o espacio...
        *q++ = '\0';       // poner fin al trozo actual

        if (*p=='>' && strcmp(trozo,"2")==0 && q==trozo+2)
        {
            redireccion = 3;   // Si hemos leído "2>", no lo
            trozo = q;         // guardamos, y tomaremos el
            p ++;              // siguiente trozo como destino
        }                      // de la salida de errores

        if (*trozo)                 // Salvo en el caso anterior,
        {                           // guardar el trozo actual
            switch (redireccion)    // donde corresponda
            {
                case 1:  O->entrada = trozo;      break;
                case 2:  O->salida = trozo;       break;
                case 3:  O->salida_err = trozo;   break;

                case 0:             // Argumento ordinario -> argv

                    if (argc+1 == argr) // Si hace falta, reservar
                    {                   // más espacio en argv
                        argr *= 2;      // (duplicar cada vez)
                        argv = (char**)
                               realloc (O->argv=argv,
                                        argr*sizeof(char**));

                        if (argv == NULL)
                            return ERROR_FALTA_MEMORIA;
                    }

                    argv[argc++] = trozo;
            }

            redireccion = 0;      // Bajar bandera de redir.
        }

        trozo = q;     // El siguiente empieza en la pos. actual

        switch (*p)    // Si carácter especial, levantar bandera
        {
            case '&':  O->segundo_plano = 1;  p ++;  break;
            case '<':  redireccion = 1;       p ++;  break;
            case '>':  redireccion = 2;       p ++;
        }

        while (*p && strchr(" \t\n",*p))  // Saltar espacio
            p ++;                         // en blanco

        if (*p == '\0')        // Fin del buffer de origen...
        {                      // se acabó
            O->argv = argv;
            O->argc = argc;
            argv[argc] = NULL;    // Marcador de fin de argv
            return 0;
        }
    }
}

/*  --------------------------------------------------------------
    liberar_orden ()

    Liberar la memoria dinámica ocupada por una órden leída por la
    lunción leer_orden().

    Parámetros:

        O           Estructura en la que leer_orden() almacenó la
                    orden.
*/

void liberar_orden (orden * O)
{
    if (O)
    {
        free (O->orden_cruda);
        free (O->buffer_trozos);
        free (O->argv);

        memset (O, 0, sizeof(orden));
    }
}

/*  --------------------------------------------------------------
    leer_linea ()

    Leer una línea de un stream (el teclado, un archivo...) y
    guardar una copia en memoria dinámica. El carácter '\n',
    que marca el final de la línea, no se almacena en el array.

    Parámetros:

        plinea      Debe apuntar a un puntero a char que, a su
                    vez, apunte a NULL. Se escribirá en él.

        stream      Origen de los datos (si NULL, se usará stdin).

    Resultado:

        Se retornará el número de caracteres leídos (sin incluir
        el '\n' de fin de línea), o un código de error (menor que
        cero).

        Además, se escribirá en *plinea (el puntero pasado por
        referencia) la dirección del nuevo array dinámico en el
        que se ha almacenado la cadena leída. El llamante tendrá
        que ocuparse de liberar esa memoria cuando ya no la
        necesite.

        Nótese que el fin de fichero y la falta de memoria se
        consideran errores, pero en ambos casos es posible que
        ya se haya reservado algo de memoria y que se hayan
        guardado datos en ella. En esos casos se retorna el
        array dinámico igual que si no hubiera ocurrido ningún
        error. En nigún caso se pierden datos.
*/

static int leer_linea (char ** plinea, FILE * stream)
{
    int tam;     // Tamaño del array reservado
    int pos;     // Posición dentro del array
    char * p;    // Puntero auxiliar para hacer el realloc
    int c;       // Carácter leído

    if (plinea == NULL || *plinea != NULL)
        return ERROR_PARAMETROS_MAL;

    if (stream == NULL)
        stream = stdin;

    tam = 8;     // Tamaño inicial (luego se irá duplicando)
    pos = 0;
    *plinea = p = (char*) malloc (tam*sizeof(char));

    if (*plinea == NULL)
        return ERROR_FALTA_MEMORIA;

    for (;;)     // Bucle infinito
    {
        c = fgetc (stream);         // Leer un carácter

        if (c=='\n' || c==EOF)      // Salto de línea o
        {                           // fin de fichero...
            p[pos] = '\0';
            return c=='\n' ? pos : ERROR_EOF; // ...se acabó
        }

        p[pos++] = c;               // Guardar carácter

        if (pos == tam)             // Si hace falta, agrandar el
        {                           // array... (duplicar tamaño)
            tam *= 2;
            p = tam<=0 ? NULL :
                (char*) realloc (*plinea, tam*sizeof(char));

            if (p == NULL)          // Si falla, *plinea sigue
            {                       // apuntando al array anterior
                p = *plinea;
                ungetc (c, stream);          // Rechazamos el últ.
                p[pos-1] = '\0';             // carácter para que
                return ERROR_FALTA_MEMORIA;  // quepa el '\0' de
            }                                // fin de cadena

            *plinea = p;  // Todo bien... apuntar al nuevo array
        }                 // y continuar leyendo caracteres
    }
}


