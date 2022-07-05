#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include <string.h>
#include <sys/socket.h>
#include "../../shared/include/shared.h"
#include <pthread.h>
#include "swap.h"
#include <math.h>
#include "manejo_memoria.h"

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

typedef struct {
    int co_op;
    int pid;
    int tamanio_proceso;
    int numero_pagina;
    int frame_libre;
    sem_t pedido_finalizado;
} pedido_swap;

t_log* logger;
Config config;
int memoria_server;
void* memoria_principal;
t_bitarray* marcos_libres;

// Listas
t_list* lista_tablas_primer_nivel;
t_list* lista_tablas_segundo_nivel;
t_list* cola_pedidos_a_swap;
t_list* lista_files;
t_list* lista_paginas_cargadas;

// Estructura Tabla Primer Nivel
typedef struct {
    int pid; // No habria que borrarlo ahora que usamos el index?
    t_list* entradas_tabla_primer_nivel;
} Tabla_Primer_Nivel;

typedef struct {
    int index_tabla_segundo_nivel;
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
    int bit_puntero;
} Entrada_Tabla_Segundo_Nivel;

//Hilo swap
pthread_t hilo_swap; 

// Mutex
pthread_mutex_t mutex_memoria;
pthread_mutex_t mutex_lista_primer_nivel;
pthread_mutex_t mutex_lista_segundo_nivel;
pthread_mutex_t mutexColaSwap;
pthread_mutex_t mutex_bitarray;

sem_t realizar_op_de_swap;
sem_t swap_esta_libre;

void cargarConfig(char*, Config*);
void inicializar_logger();
void inicializar_config();
void inicializar_servidor();
void destroy_recursos();
void inicializar_semaforos();
void inicializar_memoria_principal();
void inicializar_tablas_de_paginas();
void inicializar_swap();
char* get_file_name(int);
char* asignar_bytes(int);
int paginas_con_marco_cargado_presente(int);
int buscar_frame_libre();

#endif
