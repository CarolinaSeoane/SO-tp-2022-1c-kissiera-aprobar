#ifndef KERNEL_MAIN_H_
#define KERNEL_MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <readline/readline.h>
#include <string.h>
#include <commons/log.h>
#include "utils.h"
#include "pcb.h"
#include "serializacion.h"

void* atender_pedido(void*) ;
void mostrar_instrucciones(void*, int);
void planificador_largo_plazo(int, void*, int, Config, int);
int server_escuchar(int kernel_server);

#endif /* KERNEL_H_ */
