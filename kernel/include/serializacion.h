#ifndef SERIALIZACION_H_
#define SERIALIZACION_H_

#include <stdio.h>
#include <stdlib.h>
#include "../../shared/include/shared.h"

void send_proceso_a_cpu(PCB*, int, int);
void* serializar_proceso(PCB*, int);
int solicitar_tabla_de_paginas_a_memoria(PCB*,int);

#endif