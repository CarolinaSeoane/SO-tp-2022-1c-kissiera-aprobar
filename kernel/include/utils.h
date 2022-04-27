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
	char* IP_MEMORIA;
	char* PUERTO_MEMORIA;
	char* IP_CPU;
	char* PUERTO_CPU_DISPATCH;
	char* PUERTO_CPU_INTERRUPT;
	char* PUERTO_ESCUCHA;
	char* ALGORITMO_PLANIFICACION;
	int ESTIMACION_INICIAL;
	double ALFA;
	int GRADO_MULTIPROGRAMACION;
} Config;

typedef struct {
    int cliente_fd;
	int conexion_memoria;
    Config config;
} args_thread;

t_log* logger;

void cargarConfig(char*, Config*);

#endif
