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

void send_pedido_lectura(Proceso_CPU*, instruccion, int);
void send_pedido_escritura(int, int, int);
int recv_pedido_lectura(int);

#endif /* CONEXIONES_MEMORIA_H_ */
