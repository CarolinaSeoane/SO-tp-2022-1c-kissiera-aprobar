#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>
#include <string.h>
#include <sys/socket.h>
#include "../../shared/include/shared.h"
#include <pthread.h>

typedef struct {
    int ENTRADAS_TLB;
    char* REEMPLAZO_TLB;
    int RETARDO_NOOP;
	char* IP_MEMORIA;
    char* PUERTO_MEMORIA;
    char* PUERTO_ESCUCHA_DISPATCH;
    char* PUERTO_ESCUCHA_INTERRUPT;
} Config;

t_log* logger;
Config config;
int tamanio_pagina;
int cant_entradas_tabla;
int flag_interrupcion;
int flag_syscall;
int conexion_memoria;

int dispatch;
int interrupt;
int cliente_dispatch;
int cliente_interrupt;

pthread_mutex_t mutex_flag_interrupcion;

void cargarConfig(char*, Config*);
void inicializar_semaforos();
void inicializar_flags();
void inicializar_logger();
void inicializar_config();
void inicializar_conexiones();
void inicializar_servidores();
void destroy_recursos();

#endif /* UTILS_H_ */
