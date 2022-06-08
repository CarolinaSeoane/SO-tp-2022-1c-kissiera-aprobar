#ifndef PCB_H_
#define PCB_H_

#include <stdio.h>
#include <stdlib.h>
#include "../../shared/include/shared.h"
#include "utils.h"
#include <pthread.h>

int ULT_PID;

pthread_mutex_t mutex_PID;

int generar_pid();
void crear_pcb(PCB*, int, void*, int, int);

#endif 