#ifndef PLANIFICACION_H_
#define PLANIFICACION_H_

#include <stdio.h>
#include <stdlib.h>
#include "../../shared/include/shared.h"
#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include "pcb.h"
#include "utils.h"
#include <stdbool.h>
#include <semaphore.h>
#include "serializacion.h"
#include <unistd.h>

void* intentar_pasar_de_new_a_ready();
void pasar_de_new_a_ready(); 
void* pasar_de_ready_a_exec_FIFO();
void* pasar_de_ready_a_exec_SRT();
void pasar_de_exec_a_exit(int, int);
void* pasar_de_bloqueado_a_bloqueado_susp();
void* pasar_de_ready_susp_a_ready();
void pasar_de_exec_a_bloqueado(int, int, int);

PCB* proceso_exec;
bool hay_un_proceso_ejecutando;
bool IO_ocupado;

#endif 