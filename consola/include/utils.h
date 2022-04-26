#ifndef CONSOLA_INCLUDE_UTILS_H_
#define CONSOLA_INCLUDE_UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>
#include <string.h>
#include <sys/socket.h>
#include "../../shared/include/shared.h"

typedef struct {
	char* IP_KERNEL;
	char* PUERTO_KERNEL;
} Config;

t_log* logger;

void cargarConfig(char*, Config*);

#endif /* CONSOLA_INCLUDE_UTILS_H_ */
