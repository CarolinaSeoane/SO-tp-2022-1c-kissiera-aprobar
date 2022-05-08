#ifndef KERNEL_MAIN_H_
#define KERNEL_MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <readline/readline.h>
#include <string.h>
#include <commons/log.h>
#include <pthread.h>
#include "utils.h"
#include "pcb.h"
#include "serializacion.h"


// Listas - Estados del proceso

t_list *cola_new;
t_list *cola_ready;
t_list *cola_exec;
t_list *cola_blck;
t_list *cola_finish;

// Mutex - Estados del proceso

pthread_mutex_t mutexNew;
pthread_mutex_t mutexReady;
pthread_mutex_t mutexBlock;
pthread_mutex_t mutexExe;
pthread_mutex_t mutexExit;
pthread_mutex_t mutexBlockSuspended;
pthread_mutex_t mutexReadySuspended;



void* atender_pedido(void*) ;
void mostrar_instrucciones(void*, int);
void inicializar_colas();
void planificador_largo_plazo(int, void*, int, Config, int);
void inicializar_semaforos();

#endif /* KERNEL_H_ */
