#ifndef SERIALIZACION_H_
#define SERIALIZACION_H_

#include <stdio.h>
#include <stdlib.h>
#include "../../shared/include/shared.h"
#include "utils.h"

void send_proceso_a_cpu(PCB*, int);
void* serializar_proceso(PCB*, int);
int solicitar_tabla_de_paginas_a_memoria(PCB*);
void solicitar_swap_out_a_memoria(PCB*);
void recv_proceso_bloqueado(int*, int*, int*);
void recv_proceso_cpu(int*, int*);
void send_proceso_finalizado_a_consola(int, int);
void recv_proceso_cpu_desalojado(int*, int*);
void pedir_finalizar_estructuras_y_esperar_confirmacion(int);
void solicitar_swap_in_a_memoria(PCB*);
void esperar_confirmacion_de_swap_in();

#endif