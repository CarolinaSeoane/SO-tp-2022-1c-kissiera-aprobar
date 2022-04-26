#ifndef SERIALIZADOR_H_
#define SERIALIZADOR_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<string.h>
#include<netdb.h>
#include"protocolo.h"

// Operacion de instruccion
typedef enum {
	NO_OP,
	IO,
	READ,
	WRITE,
	COPY,
	EXIT
} operacion;

// La instruccion en si
typedef struct {
	operacion id_operacion;
	uint32_t operando1;
	uint32_t operando2;
} instruccion;

// Paquete para mandar instrucciones. Consola a Kernel
typedef struct
{
	accion id_accion;
	int length_instrucciones;
	void* stream;
} t_paquete_instrucciones;


#endif
