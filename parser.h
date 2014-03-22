/*
    parser.h
*/

#ifndef _PARSER_H_
#define _PARSER_H_

#include <stdio.h>

// Posibles códigos de error (ligados al array mensajes_err,
// cuyo valor se establece en parser.c):

#define ERROR_EOF               -1  // == EOF
#define ERROR_PARAMETROS_MAL    -2
#define ERROR_FALTA_MEMORIA     -3

// Array con los mensajes de error:

extern const char * const mensajes_err[];

// Estructura que almacenará una orden interpretada ("parseada"):

typedef struct
{
    char * orden_cruda;    // Orden completa tal como se leyó
    char * buffer_trozos;  // Orden troceada en argumentos etc.

    // NOTA: Tanto las redirecciones (entrada, salida y
    //       salida_err) como los argumentos (argv[0], argv[1],
    //       etc.) apuntan al interior del bloque buffer_trozos
    //       en vez de apuntar a bloques independientes

    char * entrada;        // Redirección de la entrada
    char * salida;         // Redirección de la salida
    char * salida_err;     // Redir. de la salida de errores

    int argc;              // Número (count) de argumentos
    char ** argv;          // Vector de argumentos

    char segundo_plano;    // 1: ejecutar en segundo plano ('&')
}                          // 0: no
orden;

// Función para inicializar una orden (hay que llamar a esta
// función antes de llamar a leer_orden()):

void inicializar_orden (orden * O);

// Función para leer e interpretar ("parsear") una orden:

int                               // 0 si OK; negativo si error
    leer_orden
               (orden * O,        // Resultado
                FILE * stream);   // Origen de los datos

// Función para liberar los recursos utilizados por una orden:

void liberar_orden (orden * O);

#endif // _PARSER_H_

