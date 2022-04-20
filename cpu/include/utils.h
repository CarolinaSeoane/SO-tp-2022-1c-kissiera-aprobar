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

t_log* logger;

void cargarConfig(char*, Config*);

#endif
