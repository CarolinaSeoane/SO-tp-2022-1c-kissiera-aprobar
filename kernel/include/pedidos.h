#ifndef PEDIDOS_H_
#define PEDIDOS_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>
#include <sys/socket.h>
#include "../../shared/include/shared.h"
#include <pthread.h>
#include "pcb.h"
#include "utils.h"
#include "planificacion.h"
#include <semaphore.h>
#include "serializacion.h"

void* atender_pedidos_consolas(void*);
void* atender_pedidos_dispatch();

#endif