#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>
#include <string.h>
#include <sys/socket.h>
#include "../../shared/include/shared.h"

typedef struct {
    int ENTRADAS_TLB;
    char* REEMPLAZO_TLB;
    int RETARDO_NOOP;
	char* IP_MEMORIA;
    char* PUERTO_MEMORIA;
    char* PUERTO_ESCUCHA_DISPATCH;
    char* PUERTO_ESCUCHA_INTERRUPT;
} Config;

typedef struct {
    int cliente_dispatch_fd;
    int cliente_interrupt_fd;
    int con_memoria;
    Config config;
} args_dispatch;

t_log* logger;
int tamanio_pagina;
int cant_entradas_tabla;
int flag_interrupcion;

void cargarConfig(char*, Config*);

#endif /* UTILS_H_ */
