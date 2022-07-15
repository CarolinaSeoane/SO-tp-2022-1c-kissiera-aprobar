#ifndef CONEXIONES_MEMORIA_H
#define CONEXIONES_MEMORIA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include "utils.h"
#include "dispatch.h"
#include "interrupt.h"
#include <pthread.h>

void send_pedido_lectura(Proceso_CPU*, int, int tlb[][3], int, int);
void send_pedido_escritura(int, uint32_t, int tlb[][3], int, int);
void send_pedido_tabla_segundo_nivel(int, int);
void send_pedido_marco(int, int, int);
uint32_t recv_pedido();

#endif /* CONEXIONES_MEMORIA_H_ */
