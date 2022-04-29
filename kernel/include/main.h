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

int atender_pedido(void*) ;
void mostrar_instrucciones(void*, int);
void inicializar_colas();

t_list *cola_new;
t_list *cola_ready;
t_list *cola_exec;
t_list *cola_blck;
t_list *cola_finish;



#endif /* KERNEL_H_ */
