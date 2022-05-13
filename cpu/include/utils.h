#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>
#include <string.h>
#include <sys/socket.h>
#include "../../shared/include/shared.h"
#include <semaphore.h>

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
    int con_memoria;
    Config config;
} args_dispatch;

typedef struct {
    int cliente_interrupt_fd;
    Config config;
} args_interrupt;

t_log* logger;
int tamanio_pagina;
int cant_entradas_tabla;
int flag_interrupcion;
int flag_syscall;

sem_t mutex_flag_interrupcion;

void cargarConfig(char*, Config*);
void inicializar_semaforo();
void inicializar_flags();

#endif /* UTILS_H_ */
