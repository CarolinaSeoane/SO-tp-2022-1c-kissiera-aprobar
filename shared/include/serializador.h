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
#include <semaphore.h>

typedef enum {
	NO_OP,
	IO,
	READ,
	WRITE,
	COPY,
	EXIT
} operacion; // Operacion de instruccion.


typedef struct {
	operacion id_operacion;
	uint32_t operando1;
	uint32_t operando2;
} instruccion; // La instruccion en si.


typedef struct {
	accion id_accion;
	int length_instrucciones;
	int tamanio_proceso;
	void* stream;
} t_paquete_instrucciones; // Paquete para mandar instrucciones. Consola a Kernel.


typedef struct {
	int pid;
	int tamanio_proceso;
	int len_instrucciones;
	void* stream;
	int program_counter;
	int tabla_paginas;
	int estimacion_rafaga;
	int cliente_fd;
	int tiempo_bloqueo;
	sem_t puedo_finalizar;
} PCB; // Estructura de un proceso.


typedef struct {
	int pid;
	void* stream;
	int program_counter;
	int tabla_paginas;
} Proceso_CPU;


#endif
