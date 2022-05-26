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
    char* PUERTO_ESCUCHA;
    int TAM_MEMORIA;
    int TAM_PAGINA;
    int ENTRADAS_POR_TABLA;
    int RETARDO_MEMORIA;
    char* ALGORITMO_REEMPLAZO;
    int MARCOS_POR_PROCESO;
    int RETARDO_SWAP;
    char* PATH_SWAP;
} Config;

typedef struct {
    int cliente_fd;
} args_thread_memoria;

t_log* logger;
Config config;
int memoria_server;

void cargarConfig(char*, Config*);
void inicializar_logger();
void inicializar_config();
void inicializar_servidor();
void destroy_recursos();

#endif
