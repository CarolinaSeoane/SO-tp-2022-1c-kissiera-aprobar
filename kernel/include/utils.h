#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>
#include <string.h>
#include <sys/socket.h>
#include "../../shared/include/shared.h"
#include <commons/collections/list.h>
#include <pthread.h>
#include "pcb.h"

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
} args_thread;

typedef struct {
	int pid;
	int cliente_fd;
} Proceso_socket;

t_log* logger;
Config config;
int kernel_server;

// Conexiones
int conexion_dispatch;
int conexion_interrupt;
int conexion_memoria;

// Listas - Estados del proceso
t_list *cola_new;
t_list *cola_ready;
t_list *cola_exec;
t_list *cola_blck;
t_list *cola_finish;
t_list *cola_suspended_ready;
t_list *cola_suspended_blck;
t_list *cola_procesos_con_socket;

// Mutex - Estados del proceso
pthread_mutex_t mutexNew;
pthread_mutex_t mutexReady;
pthread_mutex_t mutexBlock;
pthread_mutex_t mutexExe;
pthread_mutex_t mutexExit;
pthread_mutex_t mutexSuspendedBlocked;
pthread_mutex_t mutexSuspendedReady;

// Mutex - Hilos
pthread_mutex_t mutex_popular_cola_ready;
pthread_mutex_t mutex_procesos_con_socket;


void cargarConfig(char*, Config*);
void inicializar_colas();
void inicializar_semaforos();
void inicializar_logger();
void inicializar_config();
void inicializar_servidor();
void inicializar_conexiones();
void destroy_recursos();
void crear_proceso_socket(Proceso_socket*, int, int);

#endif
