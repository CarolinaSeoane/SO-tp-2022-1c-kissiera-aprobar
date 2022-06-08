#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <string.h>
#include <sys/socket.h>
#include "../../shared/include/shared.h"
#include <pthread.h>

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
void* memoria_principal;

// Listas
t_list* lista_tablas_primer_nivel;
t_list* lista_tablas_segundo_nivel;

// Estructura Tabla Primer Nivel
typedef struct {
    int pid;
    t_list* entradas_tabla_primer_nivel;
} Tabla_Primer_Nivel;

typedef struct {
    int direccion_tabla_segundo_nivel;
} Entrada_Tabla_Primer_Nivel;

// Estructura Tabla Segundo Nivel
typedef struct {
    t_list* entradas_tabla_segundo_nivel;
} Tabla_Segundo_Nivel;

typedef struct {
    int marco;
    int bit_presencia;
    int bit_modificado;
    int bit_uso;
} Entrada_Tabla_Segundo_Nivel;

// Mutex
pthread_mutex_t mutex_memoria;
pthread_mutex_t mutex_lista_primer_nivel;
pthread_mutex_t mutex_lista_segundo_nivel;

void cargarConfig(char*, Config*);
void inicializar_logger();
void inicializar_config();
void inicializar_servidor();
void destroy_recursos();
void inicializar_semaforos();
void inicializar_memoria_principal();
void inicializar_tablas_de_paginas();

#endif
